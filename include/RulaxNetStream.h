#ifndef RULAXNETSTREAM_H
#define RULAXNETSTREAM_H

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "RulaxStream.h"
#include "common.h"

using namespace std;

class RulaxNetStream :public RulaxStream {
  protected:
    int descriptor;
  public:
    RulaxNetStream(const int, unsigned long);
    ~RulaxNetStream();
    RulaxStream& operator<< (const struct transdata *);
    struct transdata * operator>> (struct transdata *);
    int getId(void);
    void closeStream(void);
};
#endif
