#include "cO_tcp.h"

// SIOOutbound methods:
int cO_tcp::busyRead()
{
    if (!connected())
        return EMPTY;

    int ch = EMPTY;

    if (chInput != 0)
    {
        ch = chInput;
        chInput = 0;
    }
    else if (!reading)
    // SetEvent(go)
    ;

    return ch;
}


uint32_t cO_tcp::tcpWorker(void *pv)
{
    return 0;
}


void cO_tcp::write(char *ptr, int bytes)
{
    if (!connected())
        return;
}

void cO_tcp::writeChar(char ch)
{
    write(&ch, 1);
}

int cO_tcp::connect(uint32_t s)
{
    return -1;
}

bool cO_tcp::connected()
{
    return false;
}

cO_tcp::cO_tcp() : reading(0)
{
    
}   


cO_tcp::~cO_tcp()
{

}
