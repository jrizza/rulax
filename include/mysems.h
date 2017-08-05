#ifndef _MYSEMS_H
#define _MYSEMS_H

#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>

union semun {
  int val;              /* Value for SETVAL */
  struct semid_ds *buf; /* Buffer for IPC_STAT, IPC_SET */
  unsigned short *array;/* Array for GETALL, SETALL */
  struct seminfo *__buf;/* Buffer for IPC_INFO (Linux specific) */
};

int semcreate(key_t, int, int, unsigned short*);
int semdestroy(key_t);
int semP(int, int);
int semV(int, int);
int semTryP(int, int);

#endif
