#ifndef GLB_SHM_OBJ_H
#define GLB_SHM_OBJ_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

struct GlbShm {
	key_t gSemKey;
	int semfd;
	int shmfd;
	int shmdatalen;
	void* shmdata;
};


int create_glb_data(int len, struct GlbShm **gData);
int write_glb_data(struct GlbShm *gData, void* data, int len);
int read_glb_data(struct GlbShm *gData, void* data, int len);
int update_glb_data(struct GlbShm *gData, void *data, int len,\
									  void (*)(struct GlbShm*, void *, int));
void destroy_glb_data(struct GlbShm *gData);

#endif /* GLB_SHM_OBJ_H */
