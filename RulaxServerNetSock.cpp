#include "include/RulaxServerNetSock.h"

RulaxServerNetSock::RulaxServerNetSock(const uint16_t port) {
  struct sockaddr_in server;

  this->port = port;

  if( (descriptor=socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("\033[31;1mRulaxServerNetSock: Error creating socket");
    exit(1);
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);
  bzero(&(server.sin_zero),8);

  int on = 1;
  setsockopt(descriptor, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );

  if(bind(descriptor,(struct sockaddr*) &server,sizeof(struct sockaddr)) == -1) {
    perror("\033[31;1mRulaxServerNetSock: Error making bind");
    exit(1);
  }
  
  if(listen(descriptor, MAX_NET_CONNECTIONS) == -1) {
    perror("\033[31;1mRulaxServerNetSock: Error making listen");
    exit(1);
  }
}

RulaxServerNetSock::~RulaxServerNetSock() {
  closeSock();
}
  
RulaxNetStream* RulaxServerNetSock::acceptConnection(void) {
  int accepted_descriptor;
  socklen_t sin_size;
  struct sockaddr_in client;

  sin_size = sizeof(struct sockaddr_in);
  accepted_descriptor = accept(descriptor, (struct sockaddr *) &client, &sin_size);
  if(accepted_descriptor == -1) {
    perror("\033[31;1mRulaxServerNetSock: Error making accept");
    exit(1);
  }

  return (new RulaxNetStream(accepted_descriptor, client.sin_addr.s_addr));
}

void RulaxServerNetSock::closeSock(void) {
  if( descriptor >=0 )
    close(descriptor);
  descriptor = -1;
}
