#include "include/RulaxLocalStream.h"

RulaxLocalStream::RulaxLocalStream(const int shmsegment, const int semset, const enum end end) {
  this->shmsegment = shmsegment;
  this->semset = semset;
  this->end = end;
  this->ip = 0;
  this->is_connected = true;
  this->message_buffer = (struct transdata *) shmat(shmsegment, NULL, 0);
}

RulaxLocalStream::~RulaxLocalStream() {
  closeStream();
}

RulaxStream& RulaxLocalStream::operator<< (const struct transdata * data) {
  //Se espera a que la otra punta haya leido
  //Se graba en memoria compartida
  //Se avisa a la otra punta que se escribio

  is_connected = (semTryP(semset, SERVER_AVAILABLE) != 0);

  if(this->end == SERVER) {
    semP(semset, CLIENT_READ);
    message_buffer[SERVER] = *data;
    semV(semset, SERVER_WROTE);
  }
  else if(this->end == CLIENT) {
    semP(semset, SERVER_READ);
    message_buffer[CLIENT] = *data;
    semV(semset, CLIENT_WROTE);
  }

  return *this; 
}

struct transdata * RulaxLocalStream::operator>> (struct transdata * data) {
  //Se espera a que la otra punta haya escrito
  //Se lee de memoria compartida
  //Se avisa a la otra punta que ya se leyo

  is_connected = (semTryP(semset, SERVER_AVAILABLE) != 0);

  if(this->end == SERVER) {
    semP(semset, CLIENT_WROTE);
    *data = message_buffer[CLIENT];
    semV(semset, SERVER_READ);
  }
  else if(this->end == CLIENT) {
    semP(semset, SERVER_WROTE);
    *data = message_buffer[SERVER];
    semV(semset, CLIENT_READ);
  }

  return data;
}

int RulaxLocalStream::getId(void) {
  return shmsegment;
}

void RulaxLocalStream::closeStream(void) {
  if(is_connected) {
    shmdt(message_buffer);
    if(this->end == CLIENT)
      semV(semset, SERVER_AVAILABLE);
  }
  is_connected = false;
}

