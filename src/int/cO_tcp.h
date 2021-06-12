#ifndef _CO_TCP_H
#define _CO_TCP_H

#include <cstdint>

#include "SIO.h"


class cO_tcp : public SIOOutbound
{
public:
    // SIOOutbound methods:
    virtual int  busyRead();
    virtual void write(char *ptr, int bytes);
    virtual void writeChar(char ch);
    virtual void onKey(bool, int, int, int) {}

    int connect(dword socket);
    int connected();

    cO_tcp();
    virtual ~cO_tcp();
private:
    uint32_t so;
    char chInput;

    // network reader thread:-
    uint32_t tcpReader();
    HANDLE thread;
    HANDLE go;
    bool reading;

    static uint32_t __stdcall tcpWorker(void *pv);
};

#endif //_CO_TCP_H
