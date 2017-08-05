#include "include/RulaxStream.h"

RulaxStream::~RulaxStream() {
}

unsigned long RulaxStream::getIp(void) {
  return ip;
}

bool RulaxStream::isConnected(void) {
  return is_connected;
}
