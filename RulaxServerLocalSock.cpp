#include "include/RulaxServerLocalSock.h"

RulaxServerLocalSock::RulaxServerLocalSock() {
  unsigned short semvals[6] = {1,0,1,0,1,0};
  mode_t mode = 0666;

  synctok = ftok("/etc/fstab",7);
  semset = semcreate(synctok, 6, IPC_CREAT|IPC_EXCL|mode, semvals);
  if(semset == -1) {
    perror("No se pueden crear los semforos. Quizas haya otro server corriendo.");
    exit(1);
  }

  shmsegment = shmget(synctok, sizeof(struct transdata) * 2, IPC_CREAT|IPC_EXCL|mode);
  if(semset == -1) {
    perror("No se pueden crear memoria compartida. Quizas haya otro server corriendo.");
    exit(1);
  }
}

RulaxServerLocalSock::~RulaxServerLocalSock() {
  closeSock();
}

RulaxLocalStream* RulaxServerLocalSock::acceptConnection() {
  semP(semset, CLIENT_CONNECTED);
  return (new RulaxLocalStream(shmsegment, semset, SERVER));
}

void RulaxServerLocalSock::closeSock() {
  if(semset >= 0) {
    semdestroy(semset);
    semset = -1;
  }
  if(shmsegment >= 0) {
    shmctl(shmsegment, IPC_RMID, NULL);
    shmsegment = -1;
  }
}
