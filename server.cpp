#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <signal.h>
#include "include/Crupier.h"
#include "include/RulaxServerConnectionManager.h"
#include "include/ServerConfig.h"

using namespace std;

void handler(int);

pid_t server_pid;
RulaxServerConnectionManager *conn_man;

int main (int argc, char* argv[]) {

  signal(SIGPIPE,SIG_IGN);
  signal(SIGINT,handler);

  server_pid = getpid();

  //Toma la configuracion del archivo
  ServerConfig options("server.cfg");

  cout<<"\033[37;1mServer.cfg"<<endl;
  cout<<"\033[37;1m--------------------"<<endl;
  cout<<"\033[37;1mPuerto: \033[30;1m"<<options.listen_port<<endl;
  cout<<"\033[37;1mJugadores: \033[30;1m"<<options.max_players<<endl;
  cout<<"\033[37;1mMonto Inicial: \033[30;1m"<<options.initial_funds<<endl;
  cout<<"\033[37;1mRecaudo de la banca: \033[30;1m"<<options.budget<<endl;
  cout<<"\033[37;1mTiempo de apuestas: \033[30;1m"<<options.bet_time<<endl;
  cout<<"\033[37;1mApuesta maxima: \033[30;1m"<<options.max_bet<<endl;
  cout<<"\033[37;1m--------------------"<<endl;
  cout<<endl;
  cout<<"\033[37;1mEsperando conexiones..."<<endl;

  bool bancarrota = false;

  PlayerManager players(options.max_players, options.initial_funds);
  Board board(options.max_bet);
  RulaxServerConnectionManager gameClients(options.listen_port, options.max_players, board, players);
  Crupier crupier(options.budget, options.bet_time, board, players, gameClients);

  conn_man = &gameClients;
  
  // El thread principal se cumple las funciones del Crupier
  unsigned int winner_number;
  srand(getpid());

  while(1) {
    while(!bancarrota) {

      //Se espera a que al menos se conecte un jugador
      gameClients.waitUntilConnectionsAvailable();
      sleep(2);
  
      gameClients.notifyStartBets();
      sleep(options.bet_time);
      gameClients.notifyStopBets();
      sleep(2);

      winner_number = rand()%37;
      cout << "\033[34;1mRULETA - Numero ganador :" << winner_number << endl;
      gameClients.notifyWinnerNumber(winner_number);

      bancarrota = crupier.payBets(winner_number);
      
      board.cleanBets();
      cout << "\033[34;1mRULETA - BUDGET $" << crupier.getBudget() << endl;
      sleep(3);
    }
    gameClients.notifyBroke();
    cout << "\033[34;1mRULETA - Bancarrota." << endl;
    crupier.setBudget(options.budget);
    bancarrota = false;
    cout << "\033[34;1mRULETA - Se repusieron los fondos." << endl;
  }
  
  return 0;
}

void handler (int signal) {
  if( getpid() == server_pid ) {
    cout<<"\033[37;1mCerrando..."<<endl;
    conn_man->stopListening();
    exit(0);
  } else {
    pthread_exit((void*) 0);
  }
}
