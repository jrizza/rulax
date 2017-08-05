#ifndef BOARD_H
#define BOARD_H

#include <map>
#include <queue>
#include "Box.h"
#include "PlayerManager.h"

using namespace std;

class Board {
  private:
    Box *boxes[118];
    queue<int> winners_list;
    unsigned int max_amount;
       /* 37 Plenos (ej: 0, 1)
       * 24 Semiplenos verticales (ej: 1-2, 2-3)
       * 33 Semiplenos horizontales (ej: 1-4, 2-5)
       * 12 Calles (ej: 1-2-3, 4-5-6)
       * 3 Docenas (ej: 1ra, 2da)
       * 3 Lineas (ej: 1-4-...-31-34, 2-5-...-32-35)
       * 2 Colores (ej: Rojo, Negro)
       * 2 Paridades (ej: Par, Impar)
       * 2 Mayores (ej: Menores, Mayores)
       */
  public:
    Board(const unsigned int=0);
    bool placeBet(const unsigned int, const unsigned int, const unsigned int);
    void cleanBets(void);
    map<int,int> getBoxBets(const unsigned int);
    multimap<int,int> getWinnerBets(const unsigned int);
    multimap<int,int> getLosserBets(const unsigned int);
    static bool boxHasNumber(const unsigned int, const unsigned int);
    void addWinnerNumber(const int);
    queue<int> getWinnersList(void);
};
#endif
