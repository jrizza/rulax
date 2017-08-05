#ifndef COMMON_H
#define COMMON_H

#define PLAYER_NAME_LENGTH 25
#define MAX_NET_CONNECTIONS 20

//Semaforos IPC
#define SERVER_AVAILABLE 0
#define CLIENT_CONNECTED 1
#define SERVER_READ 2
#define SERVER_WROTE 3
#define CLIENT_READ 4
#define CLIENT_WROTE 5

enum message_type {
  JUGADORES_INSCRIPCION, 
//  JUGADORES_PERFIL,
//  JUGADORES_DESCONECTAR,
//  SERVIDOR_REGLAS, 
  APUESTAS_COMENZAR, 
  APUESTAS_TERMINAR, 
  JUGADORES_LISTA, 
  APUESTAS_APUESTA, 
  APUESTAS_NOTIFICACION, 
  APUESTAS_PAGO, 
  RULETA_BANCARROTA, 
  JUGADORES_GANADOR, 
  JUGADORES_RETIRADA, 
  CONFIRMACION_OK, 
  CONFIRMACION_ERROR,
  SC_PLAYERS_INFORMATION,
  SC_PLAYER_ID,
  SC_PLAYER_FUNDS,
  SC_PLAYER_BET,
  SC_PLAYERS_DISCONNECT,
  SC_PLAYER_DISCONNECT,
  SC_WINNER_NUMBER,
  CS_PLAYER_QUIT
};

struct bet {
  int player_id;
  int box;
  int amount;
};

struct player {
  int id;
  char name[PLAYER_NAME_LENGTH];
  int funds;
  char color_pair;
};

struct confirmation {
  enum message_type in_response_to;
  //enum reason; //Para los errores??
};

union message_data {
  struct bet bet;
  struct player player;
  struct confirmation confirmation;
  int number;
};

struct transdata {
  enum  message_type m_type;
  union message_data m_data;
};

struct params {
  void * object1;
  int iparam1;
  unsigned long ulparam1;
};

#endif
