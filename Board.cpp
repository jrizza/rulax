#include "include/Board.h"

Board::Board(unsigned int max_amount) {

  this->max_amount = max_amount;

  int starting_box=0, i;
  // 37 Plenos (ej: 0, 1)
  for(i=0; i<37; i++)
    boxes[starting_box+i]=new Box(18, max_amount, 36);
  starting_box += i;
  // 24 Semiplenos verticales (ej: 1-2, 2-3)
  for(i=0; i<24; i++)
    boxes[starting_box+i]=new Box(5, max_amount, 18);
  starting_box += i;
  // 33 Semiplenos horizontales (ej: 1-4, 2-5)
  for(i=0; i<33; i++)
    boxes[starting_box+i]=new Box(4, max_amount, 18);
  starting_box += i;
  // 11 Calles (ej: 1-2-3, 4-5-6)
  for(i=0; i<12; i++)
    boxes[starting_box+i]=new Box(5, max_amount, 12);
  starting_box += i;
  // 3 Docenas (ej: 1ra, 2da)
  for(i=0; i<3; i++)
    boxes[starting_box+i]=new Box(18, max_amount, 3);
  starting_box += i;
  // 3 Lineas (ej: 1-4-...-31-34, 2-5-...-32-35)
  for(i=0; i<3; i++)
    boxes[starting_box+i]=new Box(18, max_amount, 3);
  starting_box += i;
  // 2 Colores (ej: Rojo, Negro)
  for(i=0; i<2; i++)
    boxes[starting_box+i]=new Box(60, max_amount, 2);
  starting_box += i;
  // 2 Paridades (ej: Par, Impar)
  for(i=0; i<2; i++)
    boxes[starting_box+i]=new Box(60, max_amount, 2);
  starting_box += i;
  // 2 Mitades (ej: Menores, Mayores)
  for(i=0; i<2; i++)
    boxes[starting_box+i]=new Box(60, max_amount, 2);
}

bool Board::placeBet(const unsigned int player_id, const unsigned int box_id, const unsigned int amount) {
  if((amount > 0 && amount <= max_amount) || !max_amount) {
    return boxes[box_id]->placeBet(player_id, amount);
  }
  return false;
}

void Board::cleanBets(void) {
  for(int i=0;i<118;i++)
    boxes[i]->grabBets();
}


map<int,int> Board::getBoxBets(const unsigned int box_id) {
  return (boxes[box_id]->getBets());
}

multimap<int,int> Board::getWinnerBets(const unsigned int winner_number) {
  map<int,int> grabbed;
  map<int,int>::iterator iter;
  multimap<int,int> recollected;
  
  for(int i=0; i<118; i++) {
    if( boxHasNumber(i, winner_number) ) {
      grabbed = boxes[i]->grabBets();
      for(iter = grabbed.begin(); iter != grabbed.end(); iter++) {
        //Se multiplica cada apuesta ganadora por la ganancia
        //No se "compactan" las diversas apuestas de un mismo usuario
        recollected.insert( pair<const int, int>(iter->first, iter->second*boxes[i]->getPayRate()) );
      }
    }
  }
  return recollected;
}
    
multimap<int,int> Board::getLosserBets(const unsigned int winner_number) {
  map<int,int> grabbed;
  map<int,int>::iterator iter;
  multimap<int,int> recollected;
  
  for(int i=0; i<118; i++) {
    if( !boxHasNumber(i, winner_number) ) {
      grabbed = boxes[i]->grabBets();
      for(iter = grabbed.begin(); iter != grabbed.end(); iter++) {
        recollected.insert( pair<const int, int>(iter->first, iter->second) );
      }
    }
  }
  return recollected;
}

bool Board::boxHasNumber(const unsigned int box_id, const unsigned int number) {
  if(box_id>=0 && box_id<=36)    //37 Plenos
    return box_id==number;
  if(number==0)
    return false;
  if(box_id>=37 && box_id<=60)   //24 Semiplenos verticales
    return (((number-1)/3==(box_id-37)/2) && ((1-((number)%3)==(box_id-37)%2) || ((number)%3==2)));
  if(box_id>=61 && box_id<=93)   //33 Semiplenos horizontales
    return (number==box_id-60 || number==box_id-60+3);
  if(box_id>=94 && box_id<=105)  //12 Calles
    return ((number-1)/3==box_id-94);
  if(box_id>=106 && box_id<=108) //3 Docenas
    return ((number-1)/12==box_id-106);
  if(box_id>=109 && box_id<=111) //3 Lineas
    return ((number-1)%3==box_id-109);
  if(box_id>=112 && box_id<=113) //2 Colores
    switch(number) {
      case 1:  case 3:  case 5:  case 7:  case 9:  case 12:
      case 14: case 16: case 18: case 19: case 21: case 23:
      case 25: case 27: case 30: case 32: case 34: case 36:
        return box_id==112;
      default:
        return box_id==113;
    }
  if(box_id>=114 && box_id<=115) //2 Paridades
    return (number%2==box_id-114);
  if(box_id>=116 && box_id<=117) //2 Mayores
    return ((number-1)%18==box_id-116);
  return false;
}

void Board::addWinnerNumber(const int winner_number) {
  if(winners_list.size()==13)
    winners_list.pop();
  winners_list.push(winner_number);
}

queue<int> Board::getWinnersList(void) {
  return winners_list;
}
