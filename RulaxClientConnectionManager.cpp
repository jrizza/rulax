#include "include/RulaxClientConnectionManager.h"

RulaxClientConnectionManager::RulaxClientConnectionManager(const char* ip, const uint16_t port, Board& board, PlayerManager& players) {
  strcpy(server_ip, ip);
  server_port = port;
  this->board = &board;
  this->players = &players;

  pthread_mutex_init(&confirmation_mutex, NULL);
  pthread_cond_init(&confirmation_arrived, NULL);
  
  RulaxClientSock *sock = new RulaxClientSock(server_ip, server_port);
  connection = sock->makeConnection();
  
  int errn;
  if((errn = pthread_create(&thread_id, NULL, listenServer, (void *)this)) != 0) {
    cout << "RulaxClientConnectionManager: Error creando el thread." << errn;
  }
}

void *RulaxClientConnectionManager::listenServer(void *caller_object) {
  struct transdata message;
  RulaxClientConnectionManager *that = (RulaxClientConnectionManager *) caller_object;
  bool player_accepted;

  //Se pone un valor invalido
  message.m_type = JUGADORES_INSCRIPCION;

  //Se espera el OK de la inscripcion
  while(that->connection->isConnected() && message.m_type != SC_PLAYERS_DISCONNECT && message.m_type != CONFIRMACION_OK && message.m_type != CONFIRMACION_ERROR) {
    *(that->connection) >> &message;

    switch(message.m_type) {
      case CONFIRMACION_OK:
      case CONFIRMACION_ERROR:
        pthread_mutex_lock(&that->confirmation_mutex);
          that->last_confirmation = message.m_type;
          that->last_confirmation_in_response_to = message.m_data.confirmation.in_response_to;
        pthread_mutex_unlock(&that->confirmation_mutex);        
        pthread_cond_broadcast(&that->confirmation_arrived);
      default:
        break;
    }
  }

  //Se comprueba que el usuario haya sido aceptado
  if(message.m_type == CONFIRMACION_OK) {
    that->interface->startInterface();
    player_accepted = true;
  } else {
    player_accepted = false;
  }

  //Se tratan los mensajes del servidor
  while(that->connection->isConnected() && message.m_type != SC_PLAYERS_DISCONNECT && player_accepted) {
    *(that->connection) >> &message;

    switch(message.m_type) {
      case SC_PLAYERS_INFORMATION:
        that->players->addPlayer(message.m_data.player.id, message.m_data.player.name, message.m_data.player.color_pair, message.m_data.player.funds);
        that->interface->drawStats();
        break;
      case SC_PLAYER_ID:
        that->players->setLocalPlayerId(message.m_data.player.id);
        break;
      case SC_PLAYER_BET:
        that->board->placeBet(message.m_data.bet.player_id, message.m_data.bet.box, message.m_data.bet.amount);
        that->interface->drawBox(message.m_data.bet.box);
        break;
      case SC_PLAYER_FUNDS:
        that->players->updatePlayerFunds(message.m_data.player.id, message.m_data.player.funds);
        that->interface->drawStats();
        if(message.m_data.player.id == that->players->getLocalPlayerId())
          that->interface->drawMyMoney();
        break;
      case SC_PLAYER_DISCONNECT:
        that->players->delPlayer(message.m_data.player.id);
        if(message.m_data.player.id == that->players->getLocalPlayerId())
          that->connection->closeStream();
        else
          that->interface->drawStats();
        break;        
      case CONFIRMACION_OK:
      case CONFIRMACION_ERROR:
        pthread_mutex_lock(&that->confirmation_mutex);
          that->last_confirmation = message.m_type;
          that->last_confirmation_in_response_to = message.m_data.confirmation.in_response_to;
        pthread_mutex_unlock(&that->confirmation_mutex);        
        pthread_cond_broadcast(&that->confirmation_arrived);
        //El protocolo indica que no se recibiran mas mensajes de confirmacion hasta atender el actual, si no fuese asi, habria que esperar algo asi como un pthread_cond_wait(confirmation_attended);
        break;
      case APUESTAS_COMENZAR:
        that->interface->cleanBoard();
        that->board->cleanBets();
        that->interface->drawWinners();
        that->interface->drawMyMoney();
        that->interface->showStartBets();
        that->interface->enableInput();
        break;
      case APUESTAS_TERMINAR:
        that->interface->disableInput();
        that->interface->showStopBets();
        break;
      case SC_WINNER_NUMBER:
        that->board->addWinnerNumber(message.m_data.number);
        that->interface->showWinnerNumber(message.m_data.number);
        break;
      case RULETA_BANCARROTA:
        that->interface->disableInput();
        that->interface->showBroke();
        sleep(3);
        break;
      default:
        break;
    }
  }

  if(player_accepted) {
    that->interface->showDisconnected();
    that->interface->enableInput();
  } else {
    cout << "Ya existe una conexion de red desde este equipo" << endl;
  }

  that->connection->closeStream();

  that->thread_return_value = 0;
  pthread_exit((void*) &(that->thread_return_value));
}

void RulaxClientConnectionManager::setInterface(Interface &interface) {
  this->interface = &interface;
}

pthread_t RulaxClientConnectionManager::getThreadId(void) {
  return thread_id;
}

bool RulaxClientConnectionManager::isConnected(void) {
  return connection->isConnected();
}

//Retorna TRUE si lo que se recibio fue un CONFIRMACION_OK y FALSE si se recibio un ERROR.
//De incluir las razones del error en el mensaje de confirmacion, el tratamiento deberia realizarse aqui o modificar la funcion (la parte de liberacion de mutex) para que dicho tratamiento se haga en la funcion que envio el mensaje original.
bool RulaxClientConnectionManager::waitConfirmation(enum message_type in_response_to) {
  bool response;
  // Se debe hacer un: pthread_mutex_lock(&confirmation_mutex); en la funcion que llame a waitConfirmation(), ANTES de enviar la informacion al servidor, si no es posible que se pierda la llegada de la confirmacion
  while(last_confirmation_in_response_to != in_response_to)
    pthread_cond_wait(&confirmation_arrived, &confirmation_mutex);
  response = (last_confirmation == CONFIRMACION_OK);

  //Se limpia la ultima confirmacion, con un mensaje que no requiera confirmacion (ej: OK, ERROR)
  last_confirmation_in_response_to = CONFIRMACION_OK;
  
  pthread_mutex_unlock(&confirmation_mutex);
  return response;
}

bool RulaxClientConnectionManager::join(const char* name) {
  struct transdata message;

  pthread_mutex_lock(&confirmation_mutex);

  message.m_type = JUGADORES_INSCRIPCION;
  strcpy(message.m_data.player.name, name);

  *connection << &message;

  return ( waitConfirmation(JUGADORES_INSCRIPCION) );
}

bool RulaxClientConnectionManager::bet(const int box, const int amount) {
  struct transdata message;
  
  pthread_mutex_lock(&confirmation_mutex);

  message.m_type = APUESTAS_APUESTA;
  message.m_data.bet.box = box;
  message.m_data.bet.amount = amount;
  *connection << &message;

  return ( waitConfirmation(APUESTAS_APUESTA) );
}

void RulaxClientConnectionManager::resign(void) {
  struct transdata message;

  message.m_type = CS_PLAYER_QUIT;

  *connection << &message;
}
