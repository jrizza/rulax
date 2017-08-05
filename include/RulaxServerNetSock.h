#ifndef RULAXSERVERNETSOCK_H
#define RULAXSERVERNETSOCK_H

#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include "RulaxNetStream.h"

using namespace std;

class RulaxServerNetSock {
  private:
    int descriptor;
    uint16_t port;
  public:
    RulaxServerNetSock(const uint16_t);
    ~RulaxServerNetSock();
    RulaxNetStream* acceptConnection(void);
    void closeSock(void);
};
#endif
