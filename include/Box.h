#ifndef BOX_H
#define BOX_H

#include <map>

using namespace std;

class Box {
  private:
    map<int,int> bets;
    unsigned int max_bets;
    unsigned int max_amount;
    unsigned int pay_rate;
  public:
    Box(const unsigned int, const unsigned int, const unsigned int);
    unsigned int getPayRate(void);
    bool placeBet(const unsigned int, const unsigned int);
    map<int,int> grabBets(void);
    map<int,int> getBets(void);
};
#endif
