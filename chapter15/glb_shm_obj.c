#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <glb_shm_obj.h>

int errexit(const char *format, ...);

// Semaphore operations
static void sem_wait(int semid, int sem_num)
{
	struct sembuf sops;
	sops.sem_num = sem_num;
	sops.sem_op = -1;
	sops.sem_flg = 0;
	if (semop(semid, &sops, 1) < 0)
		errexit("sem_wait failed: %s\n", strerror(errno));
	return ;
}

static void sem_signal(int semid, int sem_num) 
{
	struct sembuf sops;
	sops.sem_num = sem_num;
	sops.sem_op = 1;
	sops.sem_flg = 0;
	if (semop(semid, &sops, 1) < 0)
		errexit("sem_signal failed: %s\n", strerror(errno));
	return ;
}

int create_glb_data(int len, struct GlbShm **gData)
{
	int *attchErr = NULL;

	/* Allocate memory for global shared data */
	if ((*gData = (struct GlbShm*) malloc(sizeof *gData)) == NULL)
		errexit("malloc failed");
	
	/* Create shared memory IPC object */
	if (((*gData)->shmfd = shmget(IPC_PRIVATE, sizeof(len), IPC_CREAT | 0666)) < 0)
		errexit("shmget failed: %s\n", strerror(errno));
	
	/* Attach shared memory to page table of process */
	if ((attchErr = (int *)shmat((*gData)->shmfd, NULL, 0)) == (int*)-1)
		errexit("shmat failed: %s\n", strerror(errno));
	(*gData)->shmdata = (void*)attchErr;
	(*gData)->shmdatalen = len;

	/* Create semaphore IPC object */
	if (((*gData)->semfd = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666)) < 0)
		errexit("semget failed: %s\n", strerror(errno));

	/* Initialize semaphore */
	semctl((*gData)->semfd, 0, SETVAL, 1);

	/* initialize shared memory to 0 */
	memset((*gData)->shmdata, 0x0, (*gData)->shmdatalen);

	return 0;
}


int write_glb_data(struct GlbShm *gData, void* data, int len)
{
	sem_wait(gData->semfd, 0);  // Wait for semaphore	

	/* Write to shmdata */
	if (len > gData->shmdatalen)
		errexit("write to global shm failed, lenght error\n");

	memcpy(gData->shmdata, data, len);
	
	sem_signal(gData->semfd, 0);  // Release semaphore
	return 0;
}


int read_glb_data(struct GlbShm *gData, void* data, int len)
{
	sem_wait(gData->semfd, 0);  // Wait for semaphore	

	/* Read from shmdata */
	if (len > gData->shmdatalen)
		errexit("read from global shm failed, lenght error\n");

	memcpy(data, gData->shmdata, len);
	
	sem_signal(gData->semfd, 0);  // Release semaphore
	return 0;
}

int update_glb_data(struct GlbShm *gData, void *data, int len,\
									  void (*userUpdFn)(struct GlbShm*, void *, int))
{
	if (len > gData->shmdatalen)
		errexit("update to global shm failed, lenght error\n");

	sem_wait(gData->semfd, 0);  // Wait for semaphore	
	userUpdFn(gData, data, len);
	sem_signal(gData->semfd, 0);  // Release semaphore

	return 0;
}

void destroy_glb_data(struct GlbShm *gData)
{
	/* Detach and remove shared memory IPC object */
	if (shmdt(gData->shmdata) < 0)
		errexit("shmdt failed: %s\n", strerror(errno));
	if (shmctl(gData->shmfd, IPC_RMID, NULL) < 0)
		errexit("shm_unlink failed: %s\n", strerror(errno));

	/* Remove semaphore set */
	if (semctl(gData->semfd, 0, IPC_RMID) < 0)
		errexit("sem remove failed: %s\n", strerror(errno));

	free(gData);
	return ;
}
