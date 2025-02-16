#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>

#include <sys/resource.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define QLEN	5
#define BUFSIZE	4096
#define MAX(x, y)	(((x) > (y))?(x):(y))
#define	MAX_CONCUR_LVL	4

int errexit(const char *format, ...);
int passiveTCP(const char *service, const int qlen);
int echoPost2Thrd(int fd);
int echoGetFrmThrd(int idx);
int ServiceHandler(int msock);
void ChildServiceHandler();
void reaper(int);
int updateClientTbl(int ss);
int findIdxClientTbl(int ss);
int findRdFdThrdIdxClientTbl(int rd);
void clearClientTbl(int ss);

struct SConnMap {
	pid_t pid;	/* pid of slave process */
	int rd_fd;	/* read pipe descriptors for master-slave comm */
	int wr_fd;	/* write pipe descriptors for master-slave comm */
	int ssock;	/* slave connection fd */
} clientTbl[MAX_CONCUR_LVL];

int childIdx = -1; /* child process index in clientTbl */

/* Concurrent, single process, TCP server for ECHO service */

int main (int argc, char* argv[]) 
{
	char *service = "echo";
	int i, msock, pid;

	switch(argc){
		case 1:
			break;
		case 2:
			service = argv[1];
			break;
		default:
			errexit("Usage: %s [port]\n", argv[0]);
	}
	
	/* Preallocation */
	signal(SIGCHLD, reaper);
	for (i = 0 ; i < MAX_CONCUR_LVL; ++i){
		int p_rd[2], p_wr[2];
		pipe(p_rd);
		pipe(p_wr);
		switch(pid = fork()) {
			case 0: /* child/slave */
				close(p_rd[1]);
				close(p_wr[0]);
				clientTbl[i].pid = getpid();
				clientTbl[i].rd_fd = p_rd[0];
				clientTbl[i].wr_fd = p_wr[1];
				childIdx = i;
				break;
			case -1:
				errexit("Fork error: %s\n", strerror(errno));
			default: /* parent/master */
				printf("%d : Preallocation process created successfully %d\n", getpid(), pid);
				close(p_rd[0]);
				close(p_wr[1]);
				clientTbl[i].pid = pid;
				clientTbl[i].wr_fd = p_rd[1];
				clientTbl[i].rd_fd = p_wr[0];
		}
		if (pid == 0)
			break;
	}
	
	/* Parent thread */
	if (pid != 0) {
		msock = passiveTCP(service, QLEN);
		ServiceHandler(msock);
		close(msock);
		/* Cleanup Pipes for master process */
		for (i = 0 ; i < MAX_CONCUR_LVL; i++) {
			close(clientTbl[i].rd_fd);
			close(clientTbl[i].wr_fd);
			kill(clientTbl[i].pid, SIGTERM);
		}
	}
	else { /* Child thread */
		
		close(msock);
		while(1)
			ChildServiceHandler();

		/* Cleanup Pipes for slave process */
		close(clientTbl[childIdx].rd_fd);
		close(clientTbl[childIdx].wr_fd);
	}

	return 0;
}

int ServiceHandler(int msock)
{
	struct sockaddr_in fsin;
	int fd;
	fd_set afds, rfds;
	int alen, i;
	int nfds = 0;

	nfds = MAX(msock+1, nfds);
	FD_ZERO(&afds);
	FD_SET(msock, &afds);
	
	for (i = 0 ; i < MAX_CONCUR_LVL; i++) {
		FD_SET(clientTbl[i].rd_fd, &afds);
		nfds = MAX(clientTbl[i].rd_fd+1, nfds);
	}
	printf("getdtablesize: %d\n", nfds);

	while (1) {
		memcpy(&rfds, &afds, sizeof(rfds));

		if (select(nfds, &rfds, (fd_set*)0, (fd_set*)0, (struct timeval*)0) < 0) {
			if (errno != EINTR)
				errexit("Select failed: %s\n", strerror(errno));
			else
				continue;
		}
		/* Accept new connection request from master service socket */
		if (FD_ISSET(msock, &rfds)) {
			int ssock;
			alen = sizeof(fsin);
			ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
			if (ssock < 0)
				errexit("Accept failed: %s\n", strerror(errno));
			if (updateClientTbl(ssock) != 0){
				close(ssock);
			}
			else {
				FD_SET(ssock, &afds);
				nfds = MAX(ssock+1, nfds);
			}
		}
		/* Simultaneously read other connected slave sockets as well */
		for (fd = 0; fd < nfds ; ++fd)
			if (FD_ISSET(fd, &rfds) && (i = findRdFdThrdIdxClientTbl(fd)) >= 0) {
				echoGetFrmThrd(i);
			}
			else if (fd != msock && FD_ISSET(fd, &rfds))
				if (echoPost2Thrd(fd) == 0) {
					close(fd);
					FD_CLR(fd, &afds);
					clearClientTbl(fd);
				}
	}

	return 0;
}


int echoPost2Thrd(int fd)
{
	char buf[BUFSIZE];
	int cc, i;

	cc = recv(fd, buf, sizeof buf, 0);
	if (cc < 0)
		errexit("echo read failed: %s\n", strerror(errno));
	else if (cc > 0){
		if ((i = findIdxClientTbl(fd)) < 0)
			errexit("%d : Echo Client could not be found in table %d\n", getpid(), fd);

		printf("%d : Posting message to slave %d\n", getpid(), clientTbl[i].pid);
		write(clientTbl[i].wr_fd, buf, cc);
	}

	return cc;	
}

int echoGetFrmThrd(int idx)
{
	char buf[BUFSIZE];
	int cc, i;

	cc = read(clientTbl[idx].rd_fd, buf, sizeof buf);
	printf("%d : Getting message from slave %d\n", getpid(), clientTbl[idx].pid);

	if (clientTbl[idx].ssock)
		if (cc && send(clientTbl[idx].ssock, buf, cc, 0) < 0)
			errexit("echo write failed: %s\n", strerror(errno));

	return cc;
}

/* TODO: User Application Logic */
void ChildServiceHandler()
{
	char buf[BUFSIZE], tmp;
	int cc, i;

	cc = read(clientTbl[childIdx].rd_fd, buf, sizeof buf);
#if 1
	--cc; /* take away newline */
	for (i = cc -1 ; i >= cc/2 ; --i) {
		tmp = buf[i];
		buf[i] = buf[cc - 1 - i];
		buf[cc - 1 - i] = tmp;
	}
	++cc;
#endif
	write(clientTbl[childIdx].wr_fd, buf, cc);
	return ;
}

void reaper(int sig)
{
	int status, pid;
	while((pid = wait3(&status, WNOHANG, (struct rusage*)0)) > 0) {
		printf("-+Z+-Reaper read status %d, pid %d\n", WEXITSTATUS(status), pid);
	}
	return;
}

int updateClientTbl(int ss)
{
	int i;
	for(i = 0 ; i < MAX_CONCUR_LVL; ++i)
		if (clientTbl[i].ssock == 0) {
			clientTbl[i].ssock = ss;
			printf("%d : Slave thread %d assigned for client request\n", getpid(), clientTbl[i].pid);
			return 0;
		}
	printf("%d : Slave thread not available for client request\n", getpid());
	return 1;
}

void clearClientTbl(int ss)
{
	int i;
	for(i = 0 ; i < MAX_CONCUR_LVL; ++i)
		if (clientTbl[i].ssock == ss) {
			clientTbl[i].ssock = 0;
			printf("%d : Connection closed from clientTbl[%d]\n", getpid(), clientTbl[i].pid);
			return;
		}
	printf("%d : Slave sooket %d not found in clientTbl\n", getpid(), ss);
	return ;
}

int findIdxClientTbl(int ss)
{
	int i;
	for(i = 0 ; i < MAX_CONCUR_LVL; ++i)
		if (clientTbl[i].ssock == ss)
			return i;
	return -1;
}

int findRdFdThrdIdxClientTbl(int rd)
{
	int i;
	for(i = 0 ; i < MAX_CONCUR_LVL; ++i)
		if (clientTbl[i].rd_fd == rd)
			return i;
	return -1;
}
