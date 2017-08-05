#include "include/Player.h"

Player::Player(const Player& player) {
  this->id = player.id;
  strcpy(this->name, player.name); 
  this->color_pair = player.color_pair;
}

Player::Player(const int id, const char* name, const int color_pair, const int funds) {
  this->id = id;
  strcpy(this->name, name);
  this->funds = funds;
  this->color_pair = color_pair;
}

Player::~Player() {
}

char * Player::getName(void) {
  return name;
}

int Player::getFunds(void) {
  return funds;
}

void Player::setFunds(const int amount) {
  funds = amount;
}

void Player::increaseFunds(const int amount) {
  funds += amount;
}

void Player::decreaseFunds(const int amount) {
  funds -= amount;
}

int Player::getColor() {
  return color_pair;
}
