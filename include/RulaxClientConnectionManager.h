#ifndef RULAXCLIENTCONNECTIONMANAGER_H
#define RULAXCLIENTCONNECTIONMANAGER_H

#include <iostream>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include "common.h"
#include "Board.h"
#include "PlayerManager.h"
#include "RulaxClientSock.h"
#include "RulaxStream.h"
#include "Interface.h"

using namespace std;

class Interface;

class RulaxClientConnectionManager {
  private:
    char server_ip[16];
    uint16_t server_port;
    RulaxStream *connection;
    Board *board;
    PlayerManager *players;
    Interface *interface;
    pthread_t thread_id;
    int thread_return_value;
    enum message_type last_confirmation;
    enum message_type last_confirmation_in_response_to;
    pthread_mutex_t confirmation_mutex;
    pthread_cond_t confirmation_arrived;
    
    static void *listenServer(void *);
    bool waitConfirmation(enum message_type);
    
  public:
    RulaxClientConnectionManager(const char*, const uint16_t, Board&, PlayerManager&);
    void setInterface(Interface&);
    pthread_t getThreadId(void);
    bool isConnected(void);
    bool join(const char*);
    bool bet(const int, const int);
    void resign(void);
};
#endif
