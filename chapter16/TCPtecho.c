#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/time.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

int errexit(const char*, ...);
int connectTCP(const char*, const char*);
long mstime(u_long *);
int reader(int fd, fd_set *);
int writer(int fd, fd_set *);
int TCPtecho(fd_set *, int, int, int);

#define BUFSIZE	4096			/* write buffer size */
#define CCOUNT	(64*1024)	/* default character count */
#define tMAX(x, y)	((x) > (y))?(x):(y)
#define tMIN(x, y)	((x) < (y))?(x):(y)
#define USAGE	"usage: TCPtecho [-c count] host1 host2...\n"

char *hname[NOFILE];				/* fd to hostname mapping */
int rc[NOFILE], wc[NOFILE];	/* read/write character count */
char buf[BUFSIZE];

int main (int argc, char* argv[]) 
{
	int i, hcount, maxfd, fd, ccount = CCOUNT;
	fd_set afds;
	
	hcount = 0;
	maxfd = -1;
	for (i = 1; i < argc ; ++i) {
		if (strcmp(argv[i], "-c") == 0) {
			if (++i < argc && (ccount = atoi(argv[i])))
				continue;
			errexit(USAGE);
		}
		fd = connectTCP(argv[i], "echo");
		maxfd = tMAX(fd, maxfd);
		hname[fd] = argv[i];
		rc[fd] = wc[fd] = ccount;
		hcount++;
		FD_SET(fd, &afds);
	}
	TCPtecho(&afds, maxfd+1, ccount, hcount);
	return 0;
}

int TCPtecho(fd_set *afds, int nfds, int ccount, int hcount)
{
	int i, fd;
	fd_set wfds, rfds;
	u_long now;

	for (i = 0 ; i < BUFSIZE ; ++i)
		buf[i] = (i % 26) + 'a';

	mstime(NULL);
	while(hcount) {
		memcpy(&wfds, afds, sizeof(wfds));
		memcpy(&rfds, afds, sizeof(rfds));
		if (select(nfds, &rfds, &wfds, (fd_set*)0, (struct timeval*)0) < 0)
			errexit("select failed: %s\n", strerror(errno));
		for (fd = 0 ; fd < nfds ; ++fd) {
			if (FD_ISSET(fd, &wfds))
				if (0 == writer(fd, afds))
					shutdown(fd, SHUT_WR);
			if (FD_ISSET(fd, &rfds))
				if (0 == reader(fd, afds)) {
					FD_CLR(fd, afds);
					close(fd);
					hcount--;
					mstime(&now);
					printf("Test complete, elasped time for host %s : %lu ms\n", hname[fd], now);
				}
		}
	}
	return 0;
}

int writer(int fd, fd_set* rfds)
{
	int cc;

	if (wc[fd] == 0)
		return -1;
	cc = send(fd, buf, tMIN(sizeof(buf), wc[fd]), 0);
	if (cc < 0)
		errexit("write failed: %s\n", strerror(errno));
	wc[fd] -= cc;
	return wc[fd];
}

int reader(int fd, fd_set* wfds)
{
	int cc;

	if (rc[fd] == 0)
		return -1;
	cc = recv(fd, buf, sizeof(buf), 0);
	if (cc < 0)
		errexit("read failed: %s\n", strerror(errno));
	rc[fd] -= cc;
	return rc[fd];
}

long mstime(u_long *pms)
{
	static struct timeval epoch; /* older than now time */
	struct timeval now;

	if (gettimeofday(&now, (struct timeval*)0) < 0)
		errexit("gettimeofday error: %s\n", strerror(errno));

	if (!pms){
		epoch = now;
		return 0;
	}
	*pms = (now.tv_sec - epoch.tv_sec)*1000;
	*pms += (now.tv_usec - epoch.tv_usec + 500) / 1000;
	return *pms;
}
