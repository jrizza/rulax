#ifndef RULAXLOCALSTREAM_H
#define RULAXLOCALSTREAM_H

#include <sys/shm.h>
#include "mysems.h"
#include "RulaxStream.h"

using namespace std;

enum end {
  SERVER = 0,
  CLIENT = 1
};

class RulaxLocalStream :public RulaxStream {
  protected:
    struct transdata *message_buffer;
    int shmsegment;
    int semset;
    enum end end;
  public:
    RulaxLocalStream(const int, const int, const enum end);
    ~RulaxLocalStream();
    RulaxStream& operator<< (const struct transdata *);
    struct transdata * operator>> (struct transdata *);
    int getId(void);
    void closeStream(void);
};
#endif
