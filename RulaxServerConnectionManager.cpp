#include "include/RulaxServerConnectionManager.h"

RulaxServerConnectionManager::RulaxServerConnectionManager(const uint16_t port, const unsigned int max_connections, Board &board, PlayerManager &players) {
  listen_port = port;
  this->max_connections = max_connections;
  this->board = &board;
  this->players = &players;

  pthread_mutex_init(&connections_mutex, NULL);
  pthread_cond_init(&connections_available_cond, NULL);

  int errn;
  //Se crea el thread de espera para conexiones por red
  if((errn = pthread_create(&net_accepter_thread, NULL, acceptNetClients, (void *)this)) != 0) {
    cout << "\033[31;1mERROR->RulaxServerConnectionManager: Error creando thread de aceptacion de conexiones por red. " << errn;
  }

  //Se crea el thread de espera para conexiones locales
  if((errn = pthread_create(&local_accepter_thread, NULL, acceptLocalClients, (void *)this)) != 0) {
    cout << "\033[31;1mERROR->RulaxServerConnectionManager: Error creando thread de aceptacion de conexiones locales. " << errn;
  }
}

bool RulaxServerConnectionManager::waitUntilConnectionsAvailable(void) {
  
  pthread_mutex_lock(&connections_mutex);
  while(connections.size() < 1)
    pthread_cond_wait(&connections_available_cond, &connections_mutex);
  pthread_mutex_unlock(&connections_mutex);

  return true;
}

void RulaxServerConnectionManager::stopListening(void) {
  net_sock->closeSock();
  local_sock->closeSock();
}

void *RulaxServerConnectionManager::acceptNetClients(void *caller_object) {
  RulaxServerConnectionManager *that = (RulaxServerConnectionManager *) caller_object;
  RulaxNetStream *aux_stream;
  int id, errn;
  struct params params;
  
  that->net_sock = new RulaxServerNetSock(that->listen_port);
  
  //Continua infinitamente hasta que se cierre el sock
  while(1) {
    aux_stream = that->net_sock->acceptConnection();
    pthread_mutex_lock(&that->connections_mutex);
    //Se usa como ID de cliente el descriptor de stream
    id = aux_stream->getId();
    that->connections[id] = aux_stream;
    // Se crea el thread de escucha para el cliente
    params.iparam1 = id;
    params.ulparam1 = that->connections[id]->getIp();
    params.object1 = (void*)that;
    if((errn = pthread_create(&that->listener_threads[id], NULL, listenClient, (void *)&params)) != 0) {
      cout << "\033[31;1mERROR->RulaxServerConnectionManager.acceptNetClients: Error creando thread de escucha a cliente. " << errn;
    }
    //Se avisa que un usuario ingreso
    pthread_mutex_unlock(&that->connections_mutex);
    pthread_cond_broadcast(&that->connections_available_cond);
  }
  return 0; 
}

void *RulaxServerConnectionManager::acceptLocalClients(void *caller_object) {
  RulaxServerConnectionManager *that = (RulaxServerConnectionManager *) caller_object;
  RulaxLocalStream *aux_stream;
  int id, errn;
  struct params params;
  
  that->local_sock = new RulaxServerLocalSock();
  
  //Continua infinitamente hasta que se cierre el sock
  while(1) {
    aux_stream = that->local_sock->acceptConnection();
    pthread_mutex_lock(&that->connections_mutex);
    //Se usa el 0 como descriptor del stream local
    id = 0;
    that->connections[id] = aux_stream;
    // Se crea el thread de escucha para el cliente
    params.iparam1 = id;
    params.ulparam1 = that->connections[id]->getIp();
    params.object1 = (void*)that;
    if((errn = pthread_create(&that->listener_threads[id], NULL, listenClient, (void *)&params)) != 0) {
      cout << "\033[31;1mERROR->RulaxServerConnectionManager.acceptLocalClients: Error creando thread de escucha a cliente. " << errn;
    }
    //Se avisa que un usuario ingreso
    pthread_mutex_unlock(&that->connections_mutex);
    pthread_cond_broadcast(&that->connections_available_cond);
  }
  return 0; 
}

void *RulaxServerConnectionManager::listenClient(void *params) {
  set<unsigned long>::iterator it;
  struct transdata message;
  bool connection_allowed;
  struct in_addr aux_ip;

  RulaxServerConnectionManager *that = (RulaxServerConnectionManager *)((struct params *)params)->object1;
  int id = ((struct params *)params)->iparam1;
  unsigned long ip = ((struct params *)params)->ulparam1;
  aux_ip.s_addr = ip;

  //Se pone un valor invalido
  message.m_type = CONFIRMACION_OK;

  //Se comprueba que no haya un jugador conectado desde la misma IP 
  it = that->connections_ips.find( ip );
  if( it == that->connections_ips.end() ) {
    that->connections_ips.insert( ip );
    cout << "\033[32;1m(Jugador con ID " << id << " - conexion aceptada). IP: " <<inet_ntoa(aux_ip)<< endl;
    connection_allowed = true;
  } else {
    connection_allowed = false;
    cout << "\033[32;1m(Jugador con ID " << id << " - conexion duplicada desde el Host). IP: " <<inet_ntoa(aux_ip)<< endl;
  }

  //Se agrega al usuario si corresponde
  while(that->connections[id]->isConnected() && message.m_type != CS_PLAYER_QUIT && message.m_type != JUGADORES_INSCRIPCION) {
    *(that->connections[id]) >> &message;

    if(message.m_type == JUGADORES_INSCRIPCION) { 
      cout << "\033[32;1m(" << message.m_data.player.name << " entra al juego) ID: " << id << endl;

      if(connection_allowed) {
        if(that->players->addPlayer(id, message.m_data.player.name, id, that->players->getInitialFunds())) {
          that->sendConfirmation(id, CONFIRMACION_OK, JUGADORES_INSCRIPCION);
          that->sendPlayerId(id);
          that->notifyNewPlayer(id);
          that->sendPlayersList(id);
        } else {
          that->sendConfirmation(id, CONFIRMACION_ERROR, JUGADORES_INSCRIPCION);
          connection_allowed = false;
        }
      } else {
        that->sendConfirmation(id, CONFIRMACION_ERROR, JUGADORES_INSCRIPCION);
      }
    }
  }

  //Se tratan los mensajes del cliente
  while(that->connections[id]->isConnected() && message.m_type != CS_PLAYER_QUIT && connection_allowed) {
    *(that->connections[id]) >> &message;

    switch( message.m_type) {
      case APUESTAS_APUESTA:
        cout << "\033[32;1m(" << that->players->getName(id) << " - Aposto $" << message.m_data.bet.amount << " en el casillero " << message.m_data.bet.box << ")" << endl;
        if(that->board->placeBet(id, message.m_data.bet.box, message.m_data.bet.amount)) {
          that->sendConfirmation(id, CONFIRMACION_OK, APUESTAS_APUESTA);
          that->players->bill(id, message.m_data.bet.amount);
          that->notifyNewBet(id, message.m_data.bet.box, message.m_data.bet.amount);
          that->notifyPlayerFunds(id);
        } else {
          that->sendConfirmation(id, CONFIRMACION_ERROR, APUESTAS_APUESTA);
          cout << "\033[36;1mCRUPIER - Apuesta denegada" << endl;
        }
        break;
      default:
        break;
    }
  }

  //Cuando se desconecta, se avisa a todos y se elimina el jugador
  if(connection_allowed) {
    cout << "\033[32;1m(El usuario " << that->players->getName(id) << " dejo el juego)" << endl;
    that->notifyPlayerDisconnection(id);
    that->players->delPlayer(id);
    that->connections[id]->closeStream();
    pthread_mutex_lock(&that->connections_mutex);
    that->connections.erase(id);
    pthread_mutex_unlock(&that->connections_mutex);
    that->connections_ips.erase(ip);
  } else {
    cout << "\033[32;1m(El usuario con ID " << id << " dejo el juego)" << endl;
    that->connections[id]->closeStream();
    pthread_mutex_lock(&that->connections_mutex);
    that->connections.erase(id);
    pthread_mutex_unlock(&that->connections_mutex);
  }
  
  return 0;
}

void RulaxServerConnectionManager::broadcastMessage(struct transdata *message) {
  map<int,RulaxStream*>::iterator i;
  for(i = connections.begin(); i!=connections.end(); i++) {
    *(i->second) << message;
  }
}

void RulaxServerConnectionManager::sendConfirmation(const int player_id, enum message_type status, enum message_type in_response_to) {
  struct transdata message;
  message.m_type = status;
  message.m_data.confirmation.in_response_to = in_response_to;

  cout << "\033[36;1mCRUPIER - ";
  if(status == CONFIRMACION_OK)
    cout << "OK - Jugador ";
  else
    cout << "NO - Jugador ";
  cout << player_id << endl;
  *connections[player_id] << &message;
}

void RulaxServerConnectionManager::sendPlayersList(const int player_id) {
  struct transdata message;
  multimap<int, int>::iterator it;
  multimap<int, int> list;

  message.m_type = SC_PLAYERS_INFORMATION;
  
  list = players->getPlayersList();
  cout << "\033[36;1mCRUPIER - Tabla de jugadores:" << endl;

  for(it=list.begin(); it != list.end(); it++) {
    message.m_data.player.id = it->second;
    message.m_data.player.funds = it->first;
    strcpy(message.m_data.player.name, players->getName(it->second));
    message.m_data.player.color_pair = players->getColor(it->second);
    cout << "\t" << players->getName(it->second) << "\t $" << it->first << endl;

    if(it->second != player_id) 
      *connections[player_id] << &message;
  }
}

void RulaxServerConnectionManager::notifyNewPlayer(const int player_id) {
  struct transdata message;

  message.m_type = SC_PLAYERS_INFORMATION;
  message.m_data.player.id = player_id;
  message.m_data.player.funds = players->getFunds(player_id);
  strcpy(message.m_data.player.name, players->getName(player_id));
  message.m_data.player.color_pair = players->getColor(player_id);
  cout << "\033[36;1mCRUPIER - " << players->getName(player_id) << " entro al juego" << endl;

  broadcastMessage(&message);
}

void RulaxServerConnectionManager::notifyPlayerDisconnection(const int player_id) {
  struct transdata message;

  message.m_type = SC_PLAYER_DISCONNECT;
  message.m_data.player.id = player_id;
  cout << "\033[36;1mCRUPIER - Se retiro el jugador con ID " << player_id << endl;
  
  broadcastMessage(&message);
}

void RulaxServerConnectionManager::notifyNewBet(const int player_id, const unsigned int box, const unsigned int amount) {
  struct transdata message;

  message.m_type = SC_PLAYER_BET;
  message.m_data.bet.player_id = player_id;
  message.m_data.bet.box = box;
  message.m_data.bet.amount = amount;
  cout << "\033[36;1mCRUPIER - " << players->getName(player_id) << " aposto $" << amount << " en el casillero " << box << endl;

  broadcastMessage(&message);
}

void RulaxServerConnectionManager::notifyStartBets() {
  struct transdata message;
  
  message.m_type = APUESTAS_COMENZAR;
  cout << "\033[36mCRUPIER - Hagan sus apuestas!" << endl;

  broadcastMessage(&message);
}

void RulaxServerConnectionManager::notifyStopBets() {
  struct transdata message;

  message.m_type = APUESTAS_TERMINAR;
  cout << "\033[36;1mCRUPIER - NO VA MAS!!!" << endl;

  broadcastMessage(&message);
}

void RulaxServerConnectionManager::notifyBroke() {
  struct transdata message;

  message.m_type = RULETA_BANCARROTA;
  cout << "\033[36;1mCUPIER - Bancarrota." << endl;

  broadcastMessage(&message);

  message.m_type = SC_PLAYERS_DISCONNECT;

  broadcastMessage(&message);
}

void RulaxServerConnectionManager::notifyPlayerFunds(const int player_id) {
  struct transdata message;

  message.m_type = SC_PLAYER_FUNDS;
  message.m_data.player.id = player_id;
  message.m_data.player.funds = players->getFunds(player_id);
  cout << "\033[36;1mCRUPIER - Ahora " << players->getName(player_id) << " tiene $" << players->getFunds(player_id) << endl; 

  broadcastMessage(&message);
}

void RulaxServerConnectionManager::notifyWinnerNumber(const int winner_number) {
  struct transdata message;

  message.m_type = SC_WINNER_NUMBER;
  message.m_data.number = winner_number;
  if(winner_number)
     if(board->boxHasNumber(112,winner_number))  	
        cout << "\033[36;1mCRUPIER - Colorado el " << winner_number << endl;
     else
  	    cout << "\033[36;1mCRUPIER - Negro el " << winner_number << endl;
  else
  	cout << "\033[36;1mCRUPIER - Verde el " << winner_number << endl;
  
  broadcastMessage(&message);
}

void RulaxServerConnectionManager::sendPlayerId(const int player_id) {
  struct transdata message;

  message.m_type = SC_PLAYER_ID;
  message.m_data.player.id = player_id;
  cout << "\033[36;1mCRUPIER - " << players->getName(player_id) << " tu ID es " << player_id << endl;

  *connections[player_id] << &message;
}
