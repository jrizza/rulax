#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

class ServerConfig {
  public:
    short unsigned int listen_port;
    unsigned int max_players;
    unsigned int initial_funds;
    unsigned int budget;
    unsigned int bet_time;
    unsigned int max_bet;

    ServerConfig(char *);
};
#endif
