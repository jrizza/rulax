#ifndef INTERFACE_H
#define INTERFACE_H

#include <curses.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <queue>
#include "Board.h"
#include "DrawBox.h"
#include "PlayerManager.h"
#include "RulaxClientConnectionManager.h"

using namespace std;

class RulaxClientConnectionManager;

class Interface {
  private:
    WINDOW *v_screen;
    WINDOW *message_box;
    Board *board;
    PlayerManager *players;
    RulaxClientConnectionManager *connection;
    DrawBox *dboxes[118];
    int cursor_x, cursor_y;
    int screen_rows, screen_cols;
    int v_screen_rows, v_screen_cols;
    int viewport_x_offset, viewport_y_offset;
    int scroll_x_jump, scroll_y_jump;
    pthread_t thread_id;
    int thread_return_value;
    pthread_mutex_t screen_mutex, input_mutex;
    pthread_cond_t input_cond;
    bool input_enabled;
    bool interface_started;
    pthread_cond_t interface_started_cond;

    bool askForBet(int, int);
    int translateUserBet(char *);
    void translateUserBet(int, char *);
    static void *handleKeyboard(void *);
    void updateScreen(void);
    void showMessage(char*, const unsigned int, const unsigned int);
    void clearMessage(void);

  public:
    Interface(Board&, PlayerManager&);
    void startInterface(void);
    void setConnection(RulaxClientConnectionManager &);
    ~Interface();
    pthread_t getThreadId(void);
    void drawBoard(void);
    void cleanBoard(void);
    void drawBox(const int);
    void drawMyMoney(void);
    void drawStats(void);
    void drawWinners(void);
    void enableInput(void);
    void disableInput(void);
    void showStartBets(void);
    void showStopBets(void);
    void showBroke(void);
    void showDisconnected(void);
    void showWinnerNumber(const int);
};
#endif
