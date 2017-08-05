#include "include/Box.h"

Box::Box(const unsigned int max_bets, const unsigned int max_amount, const unsigned int pay_rate) {
  this->max_bets = max_bets;
  this->max_amount = max_amount;
  this->pay_rate = pay_rate;
}

unsigned int Box::getPayRate(void) {
  return pay_rate;
}

bool Box::placeBet(const unsigned int player_id, const unsigned int amount) {
  map<int,int>::iterator it;

  it = bets.find(player_id);
  if( it != bets.end() ) {
    //El jugador ya tenia apuestas en este casillero
    if(( bets[player_id] + amount <= max_amount) || !max_amount) {
      bets[player_id] = bets[player_id] + amount;
      return true;
    }
  } else if( bets.size() < max_bets ) {
    //El jugador no tenia apuestas en este casillero
    bets[player_id] = amount;
    return true;
  }
  return false;
}

map<int,int> Box::grabBets(void) {
  map<int,int>::iterator i;
  map<int,int> devolution;

  while( !bets.empty() ) {
    i = bets.begin();
    devolution.insert(*i);
    bets.erase(i);
  }
  return devolution;
}

map<int,int> Box::getBets(void) {
  return bets;
}
