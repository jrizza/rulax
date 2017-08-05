#include <stdlib.h>
#include <signal.h>
#include "include/Interface.h"
#include "include/RulaxClientConnectionManager.h"

using namespace std;

int main (int argc, char* argv[]) {
 
  signal(SIGINT, SIG_IGN);

  if(argc != 4) {
    cout <<"Uso: " << argv[0] << " SERVERIP SERVERPORT PLAYERNAME" << endl;
    exit(1);
  }

  char *server_ip = argv[1];
  short unsigned int server_port = atoi(argv[2]);
  char player_name[PLAYER_NAME_LENGTH];
  snprintf(player_name, PLAYER_NAME_LENGTH-1, "%s", argv[3]);
  int err, status;
  
  //Se crean los diferentes objetos
  PlayerManager players;
  Board board;
  //Los siguientes objetos crean cada uno un thread
  //El thread de interfaz se crea solo si la inscripcion es correcta
  RulaxClientConnectionManager gameServer(server_ip, server_port, board, players);
  Interface interface(board, players);

  //Se conectan los objetos
  gameServer.setInterface(interface);
  interface.setConnection(gameServer);

  //Se conecta con el servidor
  if( gameServer.join(player_name) ) {

    //Se espera hasta que termine los diversos threads
    err = pthread_join(interface.getThreadId(), (void**)&status);
    if(err)
      cout << "\033[31;1mERROR: Error esperando thread de Interface" << endl;
  }

  //Se espera hasta que termine los diversos threads
  err = pthread_join(gameServer.getThreadId(), (void**)&status);
  if(err)
    cout << "\033[31;1mERROR: Error esperando thread de Conexion" << endl;

  return 0;
}
