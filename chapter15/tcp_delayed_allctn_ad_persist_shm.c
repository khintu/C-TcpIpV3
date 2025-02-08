#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <netinet/in.h>

#include <unistd.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glb_shm_obj.h>

#define QLEN	2
#define BUFSIZE	4096
#define MAXCRTLVL	2

int errexit(const char *format, ...);
int passiveTCP(const char *service, const int qlen);
int serviceHandler(int msock);
int TCPechod(int fd);
void reaper(int);
void delayed_alloctn_cb(int);
void incGlbConcurryLvlCb(struct GlbShm* gData, void *data, int len);
void decGlbConcurryLvlCb(struct GlbShm* gData, void *data, int len);

jmp_buf env;
int isChild;
struct GlbShm *gConcurrcyLvl;

int main (int argc, char* argv[]) 
{
	char *service = "echo";
	int msock;
	struct sigaction sa = {0};

	switch(argc) {
		case 1:
			break;
		case 2:
			service = argv[1];
			break;
		default:
			errexit("Usage: %s [port]\n", argv[0]);
	}

	signal(SIGCHLD, reaper);
	sa.sa_handler = delayed_alloctn_cb;
	sigfillset(&sa.sa_mask);
	sa.sa_flags = SA_NODEFER;
	sigaction(SIGALRM, &sa, NULL);
	create_glb_data(sizeof(int), &gConcurrcyLvl);

	msock = passiveTCP(service, QLEN);
	serviceHandler(msock);
	close(msock);
	if (!isChild)
		destroy_glb_data(gConcurrcyLvl);
	return 0;
}

void delayed_alloctn_cb(int)
{
	int err, numCount;

#if 0 /* This check needs to be done by slave not master */
	read_glb_data(gConcurrcyLvl, (void*)&numCount, sizeof numCount);
	if (numCount > MAXCRTLVL) {
		printf("%d : cannot spawn slave, max concurrency limit reached\n", getpid());
		siglongjmp(env,2);
	}
#endif 

	err = fork(); 
	if (err == 0) {
		/* child  */
		printf("%d : slave serviceHandler starting up...\n", getpid());
		isChild = 1;
		numCount = 0;
		update_glb_data(gConcurrcyLvl, &numCount, sizeof numCount, incGlbConcurryLvlCb);
	}
	else if (err < 0) {
		errexit("Fork failed: %s\n", strerror(errno));
	}
	else
		/* parent - after forking slave, dont resume on client */
		siglongjmp(env,1);
	return;
}

int serviceHandler(int msock)
{
	struct sockaddr_in fsin;
	int ssock = 0;
	int alen;
	fd_set rfds;
	fd_set afds;
	int fd, nfds, numCount;
	struct timeval childtm = {0};

	nfds = getdtablesize();
	FD_ZERO(&afds);
	FD_SET(msock, &afds);


	while (1) {
		/* Master timeout control logic */
		if (sigsetjmp(env, 1) > 0 && ssock != 0) {
			printf("%d : Master closing client connection on timeout\n", getpid());
			close(ssock);
		}
	
		/* Slave behavior logic */
		if (isChild) {
			read_glb_data(gConcurrcyLvl, (void*)&numCount, sizeof numCount);
			if (numCount > MAXCRTLVL) {
				errexit("%d : slave shutting down, max concurrency limit reached\n", getpid());
			}
			memcpy(&rfds, &afds, sizeof rfds);
			childtm.tv_sec = 1000;childtm.tv_usec = 0;
			if ((fd = select(nfds, &rfds, (fd_set*)0, (fd_set*)0, &childtm)) < 0)
				errexit("Select failed: %s\n", strerror(errno));
			else if (fd == 0) {/* child inactivity timeout expired */
				printf("%d : slave servicerHandler shutting down, inactivity\n", getpid());
				break;
			}
		}

		alen = sizeof(fsin);
		ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
		if (ssock < 0) {
			if (errno == EINTR){
				printf("%d : Interrupt caught in accept\n", getpid());
				continue;
			}
			else
				errexit("Accept failed: %s\n", strerror(errno));
		}
		/* Delayed Alloc: User protocol logic timing code */
		if (!isChild) { /* Master times a request */
			alarm(5);
		}
		TCPechod(ssock);
		close(ssock);
		ssock = 0;
		if (!isChild) { /* Master cancels timer on request completion */
			alarm(0);
		}
	}
	return 0;
}

int TCPechod(int fd)
{
	char buf[BUFSIZE];
	int cc;

	printf("%d : New connection accepted %d\n", getpid(), fd);
	while(cc = read(fd, buf, sizeof buf)) {
		if (cc < 0 && errno == EINTR){
			printf("%d : Interrupt caught in read\n", getpid());
			continue;
		}
		else if (cc < 0)
			errexit("Read failure: %s\n", strerror(errno));
		else{
			if (write(fd, buf, cc) < 0)
				errexit("Write failure: %s\n", strerror(errno));
		}
	}
	printf("%d : connection closed %d\n", getpid(), fd);
	return 0;
}

void reaper(int sig)
{
	int status, pid, numCount;
	while((pid = wait3(&status, WNOHANG, (struct rusage*)0)) > 0) {
		printf("-+Z+-Reaper read status %d, pid %d\n", WEXITSTATUS(status), pid);
		update_glb_data(gConcurrcyLvl, &numCount, sizeof numCount, decGlbConcurryLvlCb);
	}
	return;
}

void incGlbConcurryLvlCb(struct GlbShm* gData, void *data, int len)
{
	memcpy(data, gData->shmdata, len);
	++*((int*)data);
	memcpy(gData->shmdata, data, len);
	return ;
}

void decGlbConcurryLvlCb(struct GlbShm* gData, void *data, int len)
{
	memcpy(data, gData->shmdata, len);
	--*((int*)data);
	memcpy(gData->shmdata, data, len);
	return ;
}
