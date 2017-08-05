#include "include/RulaxClientSock.h"

RulaxClientSock::RulaxClientSock(const char *ip, const uint16_t port) {
  struct hostent *serverent;

  server_port = port;
  strcpy(server_ip, ip);

  if( (serverent = gethostbyname(server_ip)) == NULL)
    perror("RulaxClientSock: Error making gethostbyname");

  int i = 0;
  do {
    network_connection =  strcmp("0.0.0.0",inet_ntoa(*((struct in_addr *)serverent->h_addr_list[i]))) *
                          strcmp("127.0.0.1",inet_ntoa(*((struct in_addr *)serverent->h_addr_list[i])));

    //cout<<"Local: "<<inet_ntoa(*((struct in_addr *)serverent_aux->h_addr_list[i]))<<endl;
    //cout<<"Servr: "<<inet_ntoa(*((struct in_addr *)serverent->h_addr_list[i]))<<endl;
    i++;
  } while( network_connection && serverent->h_addr_list[i] != NULL);

  if(!network_connection) {
    //Local Connection
    mode_t mode = 0666;

    synctok = ftok("/etc/fstab",7);
    semset = semget(synctok, 6, mode);
    if(semset == -1) {
      perror("No se puede acceder a los semforos. Asegurese de que haya un servidor corriendo en la IP especificada.");
       exit(1);
     }

    descriptor = shmget(synctok, sizeof(struct transdata) * 2, mode);
    if(descriptor == -1) {
      perror("No se puede acceder a la memoria compartida. Asegurese de que haya un servidor corriendo en la IP especificada.");
      exit(1);
    }

  } else {
    //Network Connection

    if( (descriptor=socket(AF_INET, SOCK_STREAM, 0)) == -1)
      perror("RulaxClientSock: Error creating socket");
    
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr = *((struct in_addr *) serverent->h_addr);
    bzero(&(server.sin_zero),8);
  }
}

RulaxClientSock::~RulaxClientSock() {
  if(network_connection)
    if(descriptor)
      close(descriptor);
}
  
RulaxStream* RulaxClientSock::makeConnection(void) {
  if(!network_connection) {
    //Local connection
    if(semTryP(semset, SERVER_AVAILABLE)) {
      cout << "Ya existe una conexion local desde este equipo" << endl;
      exit(1);
    }

    semP(semset, SERVER_AVAILABLE);
    RulaxLocalStream *local_stream = new RulaxLocalStream(descriptor, semset, CLIENT);
    semV(semset, CLIENT_CONNECTED);

    return local_stream;
  } else {
    //Network connection
    if(connect(descriptor, (struct sockaddr *) &server, sizeof(struct sockaddr)) == -1)
      perror("RulaxClientSock: Error making connect");

    return (new RulaxNetStream(descriptor, 0));
  }
}
