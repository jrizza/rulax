# LaRulax Makefile

CC=g++
CFLAGS=-c -Wall
LFLAGS=-lpthread -lncurses

all: server client clean_o

common: RulaxStream.o RulaxNetStream.o RulaxLocalStream.o mysems.o Box.o DrawBox.o Board.o Player.o PlayerManager.o

server: RulaxServerConnectionManager.o RulaxServerNetSock.o RulaxServerLocalSock.o Crupier.o ServerConfig.o common server.o
	$(CC) $(LFLAGS) RulaxServerConnectionManager.o RulaxStream.o RulaxNetStream.o RulaxLocalStream.o mysems.o RulaxServerNetSock.o RulaxServerLocalSock.o Box.o DrawBox.o Board.o Player.o PlayerManager.o Crupier.o ServerConfig.o server.o -o server

client: RulaxClientConnectionManager.o RulaxClientSock.o Interface.o common client.o
	$(CC) $(LFLAGS) RulaxClientConnectionManager.o RulaxStream.o RulaxNetStream.o RulaxLocalStream.o mysems.o RulaxClientSock.o Box.o DrawBox.o Board.o Player.o PlayerManager.o Interface.o client.o -o client

#Common
RulaxStream.o: RulaxStream.cpp include/RulaxStream.h include/common.h
	$(CC) $(CFLAGS) RulaxStream.cpp

RulaxNetStream.o: RulaxNetStream.cpp include/RulaxNetStream.h include/common.h
	$(CC) $(CFLAGS) RulaxNetStream.cpp

RulaxLocalStream.o: RulaxLocalStream.cpp include/RulaxLocalStream.h mysems.o include/common.h
	$(CC) $(CFLAGS) RulaxLocalStream.cpp
	
mysems.o: mysems.c include/mysems.h include/common.h
	$(CC) $(CFLAGS) mysems.c
	
Box.o: Box.cpp include/Box.h
	$(CC) $(CFLAGS) Box.cpp

Board.o: Board.cpp include/Board.h
	$(CC) $(CFLAGS) Board.cpp

Player.o: Player.cpp include/Player.h include/common.h
	$(CC) $(CFLAGS) Player.cpp

PlayerManager.o: PlayerManager.cpp include/PlayerManager.h
	$(CC) $(CFLAGS) PlayerManager.cpp

DrawBox.o: DrawBox.cpp include/DrawBox.h
	$(CC) $(CFLAGS) DrawBox.cpp

#Server	
ServerConfig.o: ServerConfig.cpp include/ServerConfig.h
	$(CC) $(CFLAGS) ServerConfig.cpp

RulaxServerConnectionManager.o: RulaxServerConnectionManager.cpp include/RulaxServerConnectionManager.h include/common.h
	$(CC) $(CFLAGS) RulaxServerConnectionManager.cpp

RulaxServerNetSock.o: RulaxServerNetSock.cpp include/RulaxServerNetSock.h include/common.h
	$(CC) $(CFLAGS) RulaxServerNetSock.cpp

RulaxServerLocalSock.o: RulaxServerLocalSock.cpp include/RulaxServerLocalSock.h include/common.h
	$(CC) $(CFLAGS) RulaxServerLocalSock.cpp

Crupier.o: Crupier.cpp include/Crupier.h
	$(CC) $(CFLAGS) Crupier.cpp

server.o: server.cpp
	$(CC) $(CFLAGS) server.cpp

#Client
RulaxClientConnectionManager.o: RulaxClientConnectionManager.cpp include/RulaxClientConnectionManager.h include/common.h
	$(CC) $(CFLAGS) RulaxClientConnectionManager.cpp

RulaxClientSock.o: RulaxClientSock.cpp include/RulaxClientSock.h include/common.h
	$(CC) $(CFLAGS) RulaxClientSock.cpp

Interface.o: Interface.cpp include/Interface.h
	$(CC) $(CFLAGS) -D_GNU_SOURCE Interface.cpp

client.o: client.cpp
	$(CC) $(CFLAGS) client.cpp

#Clean
clean:
	rm client server *.o

clean_o:
	rm *.o
