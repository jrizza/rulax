#ifndef CRUPIER_H
#define CRUPIER_H

#include "Board.h"
#include "PlayerManager.h"
#include "RulaxServerConnectionManager.h"

using namespace std;

class Crupier {
  private:
    int bet_time;
    int budget;
    Board *board;
    PlayerManager *players;
    RulaxServerConnectionManager *connections;
  public:
    Crupier(const unsigned int, const unsigned int, Board&, PlayerManager&, RulaxServerConnectionManager&);
    bool payBets(const unsigned int);
    void setBudget(const int);
    int getBudget(void);
};
#endif
