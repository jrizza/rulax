#include "include/Interface.h"

Interface::Interface(Board &board, PlayerManager &players) {

  this->board = &board;
  this->players = &players;
  this->input_enabled = false;
  this->message_box = NULL;
  this->interface_started = false;
  
  pthread_mutexattr_t mutex_attr;
  pthread_mutexattr_init(&mutex_attr);
  pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE_NP);
  pthread_mutex_init(&screen_mutex, &mutex_attr);

  pthread_mutex_init(&input_mutex, NULL);
  pthread_cond_init(&input_cond, NULL);
  pthread_cond_init(&interface_started_cond, NULL);
}

void Interface::startInterface(void) {

  initscr();
  if(has_colors())
    start_color();
  
  //COLOR_PAIRS
  //Board
  init_pair(1,COLOR_GREEN,COLOR_BLACK);
  //Mensajes
  init_pair(2,COLOR_GREEN,COLOR_BLUE);
  //Numeros
  init_pair(3,COLOR_WHITE,COLOR_GREEN);
  init_pair(4,COLOR_WHITE,COLOR_RED);
  init_pair(5,COLOR_WHITE,COLOR_BLACK);
  //Jugadores
  for(int i=0;i<8;i++) 
    init_pair(i+10,i,7-i);

  getmaxyx(stdscr, screen_rows, screen_cols);
  v_screen_rows=25;
  v_screen_cols=230;
  cursor_x=0;
  cursor_y=0;
  viewport_x_offset=0;
  viewport_y_offset=0;
  scroll_x_jump=30;
  scroll_y_jump=1;
  
  int current_box=1;
  dboxes[0] = new DrawBox(0, 3, 12, 16);
  for(int i=0; i<12; i++) {
    for(int j=0; j<3; j++) {
      dboxes[current_box] = new DrawBox(12+12*i, 3+5*(2-j), 12, 6);
      current_box++;
    }
  }
  for(int i=0; i<12; i++) {
    for(int j=0; j<2; j++) {
      dboxes[current_box] = new DrawBox(12+12*i, 3+5*(2-j), 11, 1);
      current_box++;
    }
  }
  for(int i=0; i<11; i++) {
    for(int j=0; j<3; j++) {
      dboxes[current_box] = new DrawBox(23+12*i, 3+5*(2-j), 2, 5);
      current_box++;
    }
  }
  for(int i=0; i<12; i++) {
    dboxes[current_box] = new DrawBox(12+12*i, 3, 11, 1);
    current_box++;
  }
  for(int i=0; i<3; i++) {
    dboxes[current_box] = new DrawBox(12+48*i, 0, 48, 4);
    current_box++;
  }
  for(int i=0; i<3; i++) {
    dboxes[current_box] = new DrawBox(155, 3+5*(2-i), 13, 6);
    current_box++;
  }
  for(int i=0; i<6; i++) {
    dboxes[current_box] = new DrawBox(0+28*i, 18, 28, 7);
    current_box++;
  }

  this->v_screen = newpad(v_screen_rows, v_screen_cols);

  int errn;
  if((errn = pthread_create(&thread_id, NULL, handleKeyboard, (void *)this)) != 0) {
    cout << "Error en creacion de thread HandleKeyboard " << errn;
  }

  pthread_mutex_lock(&screen_mutex);
  this->interface_started = true;
  pthread_mutex_unlock(&screen_mutex);
  pthread_cond_broadcast(&interface_started_cond);
}

pthread_t Interface::getThreadId(void) {
  
  pthread_mutex_lock(&screen_mutex);
  while(!interface_started)
    pthread_cond_wait(&interface_started_cond, &screen_mutex);
  pthread_mutex_unlock(&screen_mutex);

  return thread_id;
}

void Interface::setConnection(RulaxClientConnectionManager &connection) {
  this->connection = &connection;  
}

Interface::~Interface() {
  if(interface_started) {
    clearMessage();
    delwin(v_screen);
    endwin();
  }
}

void Interface::drawBoard(void) {

  pthread_mutex_lock(&screen_mutex);

  wattron(v_screen, COLOR_PAIR(1));

  wmove(v_screen, 0,12);  whline(v_screen, '-', 144);
  wmove(v_screen, 3, 0);  whline(v_screen, '-', 168);
  wmove(v_screen, 8,12);  whline(v_screen, '-', 156);
  wmove(v_screen,13,12);  whline(v_screen, '-', 156);
  wmove(v_screen,18, 0);  whline(v_screen, '-', 168);
  wmove(v_screen,24, 0);  whline(v_screen, '-', 168);
  wmove(v_screen, 3, 0);  wvline(v_screen, '|',  22);
  wmove(v_screen, 0,12);  wvline(v_screen, '|',  19);
  for(int i=0; i<11; i++) {
    wmove(v_screen, 3,23+i*12);  wvline(v_screen, '|',  16);
    wmove(v_screen, 3,24+i*12);  wvline(v_screen, '|',  16);
  }
  for(int i=0; i<5; i++) {
    wmove(v_screen,19,27+i*28);  wvline(v_screen, '|',  7);
    wmove(v_screen,19,28+i*28);  wvline(v_screen, '|',  7);
  }
  for(int i=0; i<2; i++) {
    wmove(v_screen,0,59+i*48);  wvline(v_screen, '|',  4);
    wmove(v_screen,0,60+i*48);  wvline(v_screen, '|',  4);
  }
  wmove(v_screen, 0,155);  wvline(v_screen, '|', 19);
  wmove(v_screen, 3,167);  wvline(v_screen, '|', 22);

  wmove(v_screen, 0,169);  whline(v_screen, '-', 59);
  wmove(v_screen, 2,169);  whline(v_screen, '-', 59);
  wmove(v_screen,24,169);  whline(v_screen, '-', 59);
  wmove(v_screen, 8,210);  whline(v_screen, '-', 19);
  wmove(v_screen,10,210);  whline(v_screen, '-', 19);
  wmove(v_screen, 0,169);  wvline(v_screen, '|', 25);
  wmove(v_screen, 0,210);  wvline(v_screen, '|', 25);
  wmove(v_screen, 0,228);  wvline(v_screen, '|', 25);
 
  wattroff(v_screen, COLOR_PAIR(1));
  wattron(v_screen, COLOR_PAIR(5));
    
  mvwprintw(v_screen, 1, 31, "1ra Docena");
  mvwprintw(v_screen, 1, 79, "2da Docena");
  mvwprintw(v_screen, 1,127, "3ra Docena");
  mvwprintw(v_screen,21, 12, "Rojo");
  mvwprintw(v_screen,21, 40, "Negro");
  mvwprintw(v_screen,21, 68, "Par");
  mvwprintw(v_screen,21, 96, "Impar");
  mvwprintw(v_screen,21,122, "Menores");
  mvwprintw(v_screen,21,150, "Mayores");
  mvwprintw(v_screen, 5,157, "1ra Linea");
  mvwprintw(v_screen,10,157, "2da Linea");
  mvwprintw(v_screen,15,157, "3ra Linea");

  mvwprintw(v_screen, 1,181, "Tabla de Jugadores");
  mvwprintw(v_screen, 1,215, "Mis Fichas");
  mvwprintw(v_screen, 3,220, "x500");
  mvwprintw(v_screen, 4,220, "x100");
  mvwprintw(v_screen, 5,220, "x25");
  mvwprintw(v_screen, 6,220, "x5");
  mvwprintw(v_screen, 7,220, "x1");
  mvwprintw(v_screen, 9,212, "Ultimos numeros");

  wattroff(v_screen, COLOR_PAIR(5));
  
  unsigned int i,j,k,l;
  unsigned int x_pos,y_pos,width,height;
  int color_pair, box_id;
  
  //Colorea el cero
  box_id = 0;
  //Se toman las dimensiones
  dboxes[box_id]->getProperties(&x_pos,&y_pos,&width,&height);
  color_pair = 3;

  wattron(v_screen, COLOR_PAIR(color_pair));
  for(k=1; k<width-1; k++)
    for(l=1; l<height-1; l++)
      mvwprintw(v_screen, y_pos+l, x_pos+k, " ");
  mvwprintw(v_screen,y_pos+height/2-1,x_pos+width/2-1, "%2d", box_id);
  wattroff(v_screen, COLOR_PAIR(color_pair));
    
  //Colorea los rojos y negros  
  for(i=0; i<12; i++) {
    for(j=0; j<3; j++) {
      box_id = 1+i*3+j;

      //Se toman las dimensiones
      dboxes[box_id]->getProperties(&x_pos,&y_pos,&width,&height);

      //Se determina el color para el casillero
      if(board->boxHasNumber(112,box_id))
        color_pair=4;
      else
        color_pair=5;
      
      wattron(v_screen, COLOR_PAIR(color_pair));
      
      for(k=1; k<width-1; k++)
        for(l=1; l<height-1; l++)
          mvwprintw(v_screen, y_pos+l, x_pos+k, " ");

      mvwprintw(v_screen,y_pos+height/2-1,x_pos+width/2-1, "%2d", box_id);
      wattroff(v_screen, COLOR_PAIR(color_pair));
    }
  }

  pthread_mutex_unlock(&screen_mutex);
  updateScreen();
}

void Interface::drawMyMoney() {
  int funds = players->getFunds(players->getLocalPlayerId());

  pthread_mutex_lock(&screen_mutex);

  mvwprintw(v_screen,3,215, "%3d", funds/500); funds-=((int)(funds/500))*500;
  mvwprintw(v_screen,4,215, "%3d", funds/100); funds-=((int)(funds/100))*100;
  mvwprintw(v_screen,5,215, "%3d", funds/25); funds-=((int)(funds/25))*25;
  mvwprintw(v_screen,6,215, "%3d", funds/5); funds-=((int)(funds/5))*5;
  mvwprintw(v_screen,7,215, "%3d", funds);

  pthread_mutex_unlock(&screen_mutex);
  updateScreen();
}

void Interface::cleanBoard(void) {
  pthread_mutex_lock(&screen_mutex);
  wclear(v_screen);
  drawBoard();
  drawStats();
  pthread_mutex_unlock(&screen_mutex);
  updateScreen();
}

void Interface::drawBox(const int box_id) {
  unsigned int x_pos,y_pos,width,height;
  int color_pair;
  char bet_aux[3];

  //Se determina el area donde dibujar apuestas segun el casillero
  dboxes[box_id]->getProperties(&x_pos,&y_pos,&width,&height);

  map<int,int> bets=board->getBoxBets(box_id);
  map<int,int>::iterator i;

  //Se dibujan las apuestas en forma de espiral, se presupone que entran
  int dx=0, dy=0;
  int xway=2, yway=0;
  int leftlimit=1, rightlimit=width-2, toplimit=0, bottomlimit=height-1;
    
  if(height>2) {
    dy=1;
    toplimit=1;
  }
  if(width>2)
    dx=1;

  for(i=bets.begin(); i!=bets.end(); i++) {
    
    pthread_mutex_lock(&screen_mutex);

      color_pair = players->getColor(i->first)%8+10;

      wattron(v_screen, COLOR_PAIR(color_pair));
      translateUserBet(i->second, bet_aux);
      mvwprintw(v_screen, y_pos+dy, x_pos+dx, "%2s", bet_aux);
      wattroff(v_screen, COLOR_PAIR(color_pair));
    pthread_mutex_unlock(&screen_mutex);
    
    dx+=xway; dy+=yway;
    if( xway && (dx>=rightlimit || dx<leftlimit) ) {
      dx-=xway;
      if(xway>0) {
        yway=1;
        rightlimit-=2;
      } else {
        yway=-1;
        leftlimit+=2;
      }
      xway=0;
      dy+=yway;
    }
    else if( yway && (dy==bottomlimit || dy<=toplimit) ) {
      dy-=yway;
      if(yway>0) {
        xway=-2;
        bottomlimit-=1;
      } else {
        xway=2;
        toplimit+=1;
      }
      yway=0;
      dx+=xway;
    }
  }
  updateScreen();
}

void Interface::drawStats(void) {
  multimap<int,int> ranking;
  multimap<int,int>::reverse_iterator it;
  int i, color_pair;
  
  ranking = players->getPlayersList();
  //Se van imprimiendo los jugadores del ranking ordenados en forma descendente por sus fondos
  pthread_mutex_lock(&screen_mutex);
 
  //Se borra la tabla anterior
  for(i=0; i<38;i++) { 
    wmove(v_screen, 3,171+i);  wvline(v_screen, ' ', 21);
  }
  
  for(it=ranking.rbegin(), i=0; it!=ranking.rend() && i < 21; it++, i++) {
      color_pair = players->getColor(it->second)%8+10;

      wattron(v_screen, COLOR_PAIR(color_pair));
      wmove(v_screen, 3+i,171); whline(v_screen, ' ', 38);
      mvwprintw(v_screen, 3+i, 171, "%2d - %s", i+1, players->getName(it->second));
      mvwprintw(v_screen, 3+i, 199, "%10d", it->first);
      wattroff(v_screen, COLOR_PAIR(color_pair));

  }
  pthread_mutex_unlock(&screen_mutex);

  updateScreen();
}

void Interface::drawWinners(void) {
  queue<int> winners;
  int number, color_pair, i;

  winners=board->getWinnersList();

  pthread_mutex_lock(&screen_mutex);
  for(i=0; !winners.empty() && i<13; i++) {
    number=winners.front();
    winners.pop();
    if(number==0)
      color_pair=3;
    else if(board->boxHasNumber(112,number))
      color_pair=4;
    else
      color_pair=5;
    wattron(v_screen, COLOR_PAIR(color_pair));
    mvwprintw(v_screen,11+i,215+(color_pair-3)*5, "%2d", number);
    wattroff(v_screen, COLOR_PAIR(color_pair));
  }
  pthread_mutex_unlock(&screen_mutex);
  updateScreen();
}

void Interface::updateScreen(void) {
  pthread_mutex_lock(&screen_mutex);
  
  wmove(v_screen, cursor_y, cursor_x);
  prefresh(v_screen, viewport_y_offset, viewport_x_offset, 0, 0, screen_rows-1, screen_cols-1);
  
  if(message_box != NULL) {
    touchwin(message_box);
    wrefresh(message_box);
  }
  pthread_mutex_unlock(&screen_mutex);
}

void *Interface::handleKeyboard(void *caller_object) {
  Interface *that = (Interface *) caller_object;
  int key;

  that->drawBoard();
  that->showMessage("Espere...", 3, 30);
  
  pthread_mutex_lock(&that->input_mutex);
  while(!that->input_enabled)
    pthread_cond_wait(&that->input_cond,&that->input_mutex);
  pthread_mutex_unlock(&that->input_mutex);

  keypad(that->v_screen, true);
  key = 0;

  while(that->connection->isConnected()) {

    pthread_mutex_lock(&that->input_mutex);
    while(!that->input_enabled) {
      key = 0;
      pthread_cond_wait(&that->input_cond,&that->input_mutex);
    }
    pthread_mutex_unlock(&that->input_mutex);
    
    switch(key) {
      case KEY_HOME:
        that->cursor_x = 0;
        that->viewport_x_offset = 0;
        break;
      case KEY_END:
        that->cursor_x = that->v_screen_cols - that->screen_cols - 1;
        that->viewport_x_offset = that->v_screen_cols - that->screen_cols - 1;
        break;
      case KEY_NPAGE:
        if(that->cursor_x + that->scroll_x_jump < that->v_screen_cols) that->cursor_x += that->scroll_x_jump;
        break;
      case KEY_PPAGE:
        if(that->cursor_x-that->scroll_x_jump >= 0) that->cursor_x -= that->scroll_x_jump;
        else that->cursor_x = 0;
        break;
      case KEY_UP:
        if(that->cursor_y > 0) that->cursor_y--;
        break;
      case KEY_DOWN:
        if(that->cursor_y < that->v_screen_rows - 1) that->cursor_y++;
        break;
      case KEY_RIGHT:
        if(that->cursor_x < that->v_screen_cols - 2) that->cursor_x++;
        break;
      case KEY_LEFT:
        if(that->cursor_x > 0) that->cursor_x--;
        break;
      case KEY_ENTER: case 10:
        that->askForBet(that->cursor_x, that->cursor_y);
        break;
      case 27: //ESC
        that->connection->resign();
        key = 0;
        break;
    }
 
    //Se recalcula la posicion para el Scroll
    while(that->cursor_x > (that->viewport_x_offset + that->screen_cols - 1) ) {
      if(that->viewport_x_offset + that->scroll_x_jump < that->v_screen_cols-that->screen_cols - 1)
        that->viewport_x_offset += that->scroll_x_jump;
      else
        that->viewport_x_offset = that->v_screen_cols - that->screen_cols - 1;
    }
    while(that->cursor_x < that->viewport_x_offset ) {
      if(that->cursor_x-that->scroll_x_jump >= 0)
        that->viewport_x_offset = that->cursor_x - that->scroll_x_jump;
      else
        that->viewport_x_offset = 0;
    }
    while(that->cursor_y > (that->viewport_y_offset + that->screen_rows - 1) ) {
      if(that->viewport_y_offset + that->scroll_y_jump < that->v_screen_rows-that->screen_rows - 1)
        that->viewport_y_offset += that->scroll_y_jump;
      else
        that->viewport_y_offset = that->v_screen_rows - that->screen_rows;
    }
    while(that->cursor_y < that->viewport_y_offset ) {
      if(that->cursor_y - that->scroll_y_jump >= 0)
        that->viewport_y_offset = that->cursor_y - that->scroll_y_jump;
      else
        that->viewport_y_offset = 0;
    }

    //Se posiciona el cursor y se actualiza la pantalla
    if(key!=0)
      that->updateScreen();

    flushinp();
    key = wgetch(that->v_screen);
  }

  that->thread_return_value = 0;
  pthread_exit((void *) &(that->thread_return_value));
}

bool Interface::askForBet(int x, int y) {
  WINDOW* ask_pad;
  char user_input[3];
  int selected_box;
  int bet_amount;

  //Se busca cual es el casillero sobre el que se presiono ENTER
  for(selected_box=0; selected_box<118; selected_box++)
    if( dboxes[selected_box]->isInside(x,y) ) break;
  if( selected_box==118 )
    return false;
  pthread_mutex_lock(&screen_mutex);

  ask_pad = newwin(3, 30, (screen_rows-3)/2, (screen_cols-30)/2);
  wbkgd(ask_pad, COLOR_PAIR(2));
  mvwprintw(ask_pad, 1, 2, "Monto a apostar:");

  do {
    wmove(ask_pad, 1, 19);
    whline(ask_pad, '-', 2);
    flushinp();
    mvwgetnstr(ask_pad, 1, 19, user_input, 2);
    bet_amount = translateUserBet(user_input);
  } while (bet_amount < 0);

  delwin(ask_pad);
  pthread_mutex_unlock(&screen_mutex);

  int funds = players->getFunds(players->getLocalPlayerId());

  if(funds < bet_amount) {
    showMessage("ERROR - Fondos insuficientes", 3, 30);
    flash();
    sleep(1);
    clearMessage();
    return false;
  }
  else if(!connection->bet(selected_box, bet_amount)) {
    showMessage("ERROR - Apuesta maxima superada", 3, 30);
    flash();
    sleep(1);
    clearMessage();
    return false;
  }
  return true;
}

int Interface::translateUserBet(char* user_input) {
  int bet_amount;

  char decs = user_input[0];
  char units = user_input[1];

  if( decs>='0' && decs<='9' )
    bet_amount = (decs-'0') * 10;
  else 
    return -1;

  if( units>='0' && units<='9' )
    bet_amount += (units-'0');
  else if( units == 'C' || units == 'c' )
    bet_amount *= 10;
  else if( units == '\0' )
    bet_amount = bet_amount/10;
  else
    return -1;

  return bet_amount;
}

void Interface::translateUserBet(int input, char* output) {
  output[2] = '\0';

  if( input < 100) {
    sprintf( output, "%2d", input);
  } else {
    output[0] = input/100 + '0';
    output[1] = 'C';
  }
}

void Interface::showMessage(char* text, const unsigned int h, const unsigned int w) {
  flushinp();

  pthread_mutex_lock(&screen_mutex);

  clearMessage();
  message_box = newwin(h, w, (screen_rows-h)/2, (screen_cols-w)/2);
  wbkgd(message_box, COLOR_PAIR(2));

  mvwprintw(message_box, h/2, (w-strlen(text))/2, text);
  pthread_mutex_unlock(&screen_mutex);
  updateScreen();
}

void Interface::clearMessage(void) {
  pthread_mutex_lock(&screen_mutex);
  if(message_box != NULL)
    delwin(message_box);
  message_box = NULL;
  pthread_mutex_unlock(&screen_mutex);
  updateScreen();
}

void Interface::enableInput(void) {
  pthread_mutex_lock(&input_mutex);
  input_enabled=true;
  pthread_mutex_unlock(&input_mutex);
  pthread_cond_broadcast(&input_cond);
}

void Interface::disableInput(void) {
  pthread_mutex_lock(&input_mutex);
  input_enabled=false;
  pthread_mutex_unlock(&input_mutex);
}

void Interface::showStartBets(void) {
  showMessage("Hagan sus apuestas...", 3, 30);
  sleep(1);
  clearMessage();
}

void Interface::showStopBets(void) {
  showMessage("NO VA MAS!!!", 3, 30);
}

void Interface::showBroke(void) {
    showMessage("GAME OVER - La casa se fundio!", 3, 30);
}

void Interface::showDisconnected(void) {
  showMessage("Desconectando...", 3, 30);
  sleep(2);
  showMessage("Presione una tecla para salir",3,34);
}

void Interface::showWinnerNumber(const int winner_number) {
  char text[30];
  sprintf(text, "Numero ganador: %d", winner_number);
  showMessage(text, 3, 30);
}
