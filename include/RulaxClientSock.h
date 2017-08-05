#ifndef RULAXCLIENTSOCK_H
#define RULAXCLIENTSOCK_H

#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/shm.h>
#include "mysems.h"
#include "RulaxLocalStream.h"
#include "RulaxNetStream.h"

using namespace std;

class RulaxClientSock {
  private:
    uint16_t server_port;
    char server_ip[16];
    struct sockaddr_in server;
    int descriptor;
    int synctok;
    int semset;
    bool network_connection;
  public:
    RulaxClientSock(const char*, const uint16_t);
    ~RulaxClientSock();
    RulaxStream* makeConnection(void);
};
#endif
