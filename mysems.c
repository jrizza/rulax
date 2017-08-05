#include "include/mysems.h"

int semcreate(key_t key, int nsems, int semflg, unsigned short* values) {
  int semset, ctlret, i, initialize;
  union semun params;

  semset = semget(key, nsems, semflg|IPC_EXCL);
  //Si la creacion no indica IPC_EXCL solo el primer proceso que crea los semaforos debe inicializarlos
  if(!(nsems&IPC_EXCL) && (semset==-1) && (errno==EEXIST)) {
    semset = semget(key, nsems, semflg);
    initialize=0;
  } else {
    initialize=1;
  }
    
  if(semset == -1 && !(nsems&IPC_EXCL))
    return semset;

  if(initialize) {
    params.array = (unsigned short*) malloc( nsems * sizeof(unsigned short) );
    if(params.array == NULL)
      return -1;

    for(i=0;i<nsems;i++)
      params.array[i] = values[i];
 
    ctlret = semctl(semset, nsems, SETALL, params);
    if(ctlret != 0)
      return ctlret;
  }
  return semset;
}

int semdestroy(key_t key) {
  return semctl(key,0,IPC_RMID,0);
}

int semP(int semset, int sem) {
  struct sembuf semoperation;
  semoperation.sem_num = sem;
  semoperation.sem_op = -1;
  semoperation.sem_flg = 0;
  return semop(semset, &semoperation, 1);
}

int semV(int semset, int sem) {
  struct sembuf semoperation;
  semoperation.sem_num = sem;
  semoperation.sem_op = 1;
  semoperation.sem_flg = 0;
  return semop(semset, &semoperation, 1);
}

//Retorna 0 si puede tomarse el semaforo
int semTryP(int semset, int sem) {
  union semun params;
  
  return 0 == semctl(semset, sem, GETVAL, params) ;
}
