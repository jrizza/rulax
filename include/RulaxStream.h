#ifndef RULAXSTREAM_H
#define RULAXSTREAM_H

#include <unistd.h>
#include "common.h"

using namespace std;

class RulaxStream {
  protected:
    unsigned long ip;
    bool is_connected;
    
  public:
    virtual ~RulaxStream();
    virtual RulaxStream& operator<< (const struct transdata *)=0;
    virtual struct transdata* operator>> (struct transdata *)=0;
    virtual int getId(void)=0;
    unsigned long getIp(void);
    virtual void closeStream(void)=0;
    bool isConnected(void);
};
#endif
