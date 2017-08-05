#include "include/PlayerManager.h"

PlayerManager::PlayerManager(const unsigned int max_players, const unsigned int initial_funds) {
  this->max_players = max_players;
  this->initial_funds = initial_funds;
  this->local_player_id = 0;
}

bool PlayerManager::addPlayer(const int id, const char* name, const int color_pair, int funds) {
  if( players.size() <= max_players || !max_players) {
    players[id] = new Player(id, name, color_pair, funds);
    return true;
  }
  return false;
}

void PlayerManager::delPlayer(const int id) {
  delete players[id];
  players.erase(id);
}

int PlayerManager::getLocalPlayerId(void) {
  return local_player_id;
}

void PlayerManager::setLocalPlayerId(const int id) {
  local_player_id=id;
}

bool PlayerManager::updatePlayerFunds(const int id, const int funds) {
  players[id]->setFunds(funds);
  return true;
}

int PlayerManager::getInitialFunds(void) {
  return initial_funds;
}

char* PlayerManager::getName(const int id) {
  return players[id]->getName();
}

int PlayerManager::getColor(const int id) {
  return players[id]->getColor();
}

int PlayerManager::getFunds(const int id) {
  return players[id]->getFunds();
}

multimap<int,int> PlayerManager::getPlayersList(void) {
  multimap<int,int> ranking;
  multimap<int,Player*>::iterator it;
  int player_id;
  int funds;
  
  for(it=players.begin(); it!=players.end(); it++) {
    player_id=it->first;
    funds=players[player_id]->getFunds();
    ranking.insert(pair<const int, int>(funds, player_id));
  }
  //Atencion: el map devuelto esta ordenado por fondos, no por user_id
  return ranking;
}

void PlayerManager::pay(const int id, const unsigned int amount) {
  players[id]->increaseFunds(amount);
}

void PlayerManager::bill(const int id, const unsigned int amount) {
  players[id]->decreaseFunds(amount);
}
