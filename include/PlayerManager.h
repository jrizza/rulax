#ifndef PLAYERMANAGER_H
#define PLAYERMANAGER_H

#include <map>
#include "Player.h"

using namespace std;

class PlayerManager {
  private:
    map<int,Player*> players;
    unsigned int max_players;
    unsigned int initial_funds;
    int local_player_id;
  public:
    PlayerManager(const unsigned int =0, const unsigned int =0);
    bool addPlayer(const int, const char*, const int, const int);
    void delPlayer(const int);
    int getLocalPlayerId(void);
    void setLocalPlayerId(const int);
    bool updatePlayerFunds(const int, const int);
    int getInitialFunds(void);
    char* getName(const int);
    int getColor(const int);
    int getFunds(const int);
    multimap<int,int> getPlayersList(void);
    void pay(const int, const unsigned int);
    void bill(const int, const unsigned int);
};
#endif
