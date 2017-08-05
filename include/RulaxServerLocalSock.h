#ifndef RULAXSERVERLOCALSOCK_H
#define RULAXSERVERLOCALSOCK_H

#include <stdio.h>
#include <sys/shm.h>
#include "mysems.h"
#include "RulaxLocalStream.h"

using namespace std;

class RulaxServerLocalSock {
  private:
    int synctok;
    int semset;
    int shmsegment;
  public:
    RulaxServerLocalSock();
    ~RulaxServerLocalSock();
    RulaxLocalStream* acceptConnection(void);
    void closeSock(void);
};
#endif
