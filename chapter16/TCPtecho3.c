#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

int errexit(const char*, ...);
int connectTCP(const char*, const char*);
long mstime(u_long *);
int reader(int fd);
int writer(int fd);
int TCPtecho(fd_set *, int, int, int);
void reaper(int);

#define BUFSIZE	4096			/* write buffer size */
#define CCOUNT	(64*1024)	/* default character count */
#define tMAX(x, y)	((x) > (y))?(x):(y)
#define tMIN(x, y)	((x) < (y))?(x):(y)
#define USAGE	"usage: TCPtecho [-c count] host1 host2...\n"

char *hname[NOFILE];				/* fd to hostname mapping */
int rc[NOFILE], wc[NOFILE];	/* read/write character count */
char buf[BUFSIZE];
int hcount; 

int main (int argc, char* argv[]) 
{
	int i, pid, maxfd, fd, ccount = CCOUNT;
	fd_set afds, cfds;
	
	signal(SIGCHLD, reaper);
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
	for (i = 0 ; i <= maxfd  ; ++i) {
		if (FD_ISSET(i, &afds)) {
			FD_ZERO(&cfds);
			FD_SET(i, &cfds);
			if ((pid = fork()) == 0) {
				TCPtecho(&cfds, maxfd+1, ccount, hcount);
				return 0;
			}
			else if (pid < 0)
				errexit("Fork error: %s\n", strerror(errno));
			printf("%d : Lauching writer child successfully\n", getpid());
		}
	}

	while (hcount) {
		// Do other stuff in parent pid while waiting for Inp/Outp slaves to end
		usleep(15000);
	}
	printf("%d : Tests complete, shutting down\n", getpid());
	return 0;
}

int TCPtecho(fd_set *afds, int nfds, int ccount, int hcount)
{
	int i, fd, flag = 0;
	fd_set wfds, rfds;
	u_long now;

	for (i = 0 ; i < BUFSIZE ; ++i)
		buf[i] = (i % 26) + 'a';

	mstime(NULL);
	while(!flag) {
		memcpy(&wfds, afds, sizeof(wfds));
		memcpy(&rfds, afds, sizeof(rfds));
		if (select(nfds, &rfds, &wfds, (fd_set*)0, (struct timeval*)0) < 0)
			errexit("select failed: %s\n", strerror(errno));
		for (fd = 0 ; fd < nfds ; ++fd) {
			if (FD_ISSET(fd, &wfds))
				if (0 == writer(fd))
					shutdown(fd, SHUT_WR);
			if (FD_ISSET(fd, &rfds))
				if (0 == reader(fd)) {
					FD_CLR(fd, afds);
					close(fd);
					mstime(&now);
					printf("%d : Test complete, elasped time for host %s : %lu ms\n", getpid(), hname[fd], now);
					flag = 1;
					break;
				}
		}
	}
	return 0;
}

int writer(int fd)
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

int reader(int fd)
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

void reaper(int sig)
{
	int status, pid;
	while((pid = wait3(&status, WNOHANG, (struct rusage*)0)) > 0) {
		printf("-+Z+-Reaper read status %d, pid %d\n", WEXITSTATUS(status), pid);
		--hcount;
	}
	return;
}
