#ifndef RULAXSERVERCONNECTIONMANAGER_H
#define RULAXSERVERCONNECTIONMANAGER_H

#include <map>
#include <set>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <iostream>
#include "common.h"
#include "Board.h"
#include "PlayerManager.h"
#include "RulaxServerLocalSock.h"
#include "RulaxServerNetSock.h"
#include "RulaxStream.h"
#include "RulaxLocalStream.h"
#include "RulaxNetStream.h"

using namespace std;

class RulaxServerConnectionManager {
  private:
    uint16_t listen_port;
    RulaxServerNetSock* net_sock;
    RulaxServerLocalSock* local_sock;
    map<int,RulaxStream*> connections;
    set<unsigned long> connections_ips;
    unsigned int max_connections;
    pthread_t net_accepter_thread;
    pthread_t local_accepter_thread;
    pthread_mutex_t connections_mutex;
    pthread_cond_t connections_available_cond;
    map<int,pthread_t> listener_threads;
    Board *board;
    PlayerManager *players;
    
    static void *listenClient(void *);
    static void *acceptNetClients(void *);
    static void *acceptLocalClients(void *);
    void broadcastMessage(struct transdata *);
    void sendConfirmation(const int, enum message_type, enum message_type);
    void sendPlayersList(const int);
    void sendPlayerId(const int);
    void notifyNewPlayer(const int);
    void notifyPlayerDisconnection(const int);
    void notifyNewBet(const int, const unsigned int, const unsigned int);
    
  public:
    RulaxServerConnectionManager(const uint16_t, const unsigned int, Board&, PlayerManager&);
    bool waitUntilConnectionsAvailable(void);
    void stopListening(void);
    void notifyStartBets();
    void notifyStopBets();
    void notifyPlayerFunds(const int);
    void notifyBroke();
    void notifyWinnerNumber(const int);
};
#endif
