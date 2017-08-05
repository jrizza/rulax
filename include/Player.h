#ifndef PLAYER_H
#define PLAYER_H

#include <string.h>
#include "common.h"

using namespace std;

class Player {
  private:
    int id;
    char name[PLAYER_NAME_LENGTH];
    int color_pair;
    int funds;
  public:
    Player(const Player&);
    Player(const int, const char*, const int, const int);
    ~Player();
    char* getName(void);
    int getFunds(void);
    void setFunds(const int);
    void increaseFunds(const int);
    void decreaseFunds(const int);
    int getColor();
};
#endif
