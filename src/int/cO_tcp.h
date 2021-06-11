#if !defined(CO_TCP_H)
#define CO_TCP_H

#include <cstdint>
#include <sys/socket.h>

#include "SIO.h"

class cO_tcp : public SIOOutbound
{
public:
    // SIOOutbound methods:
    virtual int  busyRead();
    virtual void write(char *ptr, int bytes);
    virtual void writeChar(char ch);
    virtual void onKey(bool, int, int, int) {}

    int connect(uint32_t socket);
    bool connected();

    cO_tcp();
    virtual ~cO_tcp();
private:

    char chInput;

    bool reading;

    static uint32_t tcpWorker(void *pv);
};

#endif

