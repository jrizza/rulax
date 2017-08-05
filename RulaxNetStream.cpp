#include "include/RulaxNetStream.h"

RulaxNetStream::RulaxNetStream(const int descriptor, unsigned long ip) {
  this->descriptor = descriptor;
  this->ip = ip;
  this->is_connected = true;
}

RulaxNetStream::~RulaxNetStream() {
  closeStream();
}

RulaxStream& RulaxNetStream::operator<< (const struct transdata * data) {
  this->is_connected = (send(descriptor, data, sizeof(struct transdata), 0) > 0);
  return *this;
}

struct transdata * RulaxNetStream::operator>> (struct transdata * data) {
  this->is_connected = (recv(descriptor, data, sizeof(struct transdata), MSG_WAITALL) > 0);
  return data;
}

int RulaxNetStream::getId(void) {
  return descriptor;
}

void RulaxNetStream::closeStream(void) {
  if(is_connected)
    close(descriptor);
  is_connected = false;
}
