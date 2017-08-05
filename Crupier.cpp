#include "include/Crupier.h"

Crupier::Crupier(const unsigned int budget, const unsigned int bet_time, Board& board, PlayerManager& players, RulaxServerConnectionManager& connections) {
  this->budget = budget;
  this->bet_time = bet_time;
  this->board = &board;
  this->players = &players;
  this->connections = &connections;
}

void Crupier::setBudget(const int budget) {
  this->budget = budget;
}
int Crupier::getBudget(void) {
  return budget;
}

bool Crupier::payBets(const unsigned int winner_number) {
  multimap<int,int> recollected;
  multimap<int,int>::iterator iter;
  
  recollected = board->getLosserBets(winner_number);
  for(iter = recollected.begin(); iter != recollected.end(); iter++) {
    budget += iter->second;
  }
  
  recollected = board->getWinnerBets(winner_number);
  for(iter = recollected.begin(); iter != recollected.end(); iter++) {
    budget -= iter->second;
    if(budget <= 0) {
      return true;
    } else {
      players->pay(iter->first, iter->second);
      connections->notifyPlayerFunds(iter->first);
    }
  }
  return false;
}
