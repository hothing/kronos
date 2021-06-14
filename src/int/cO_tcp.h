#ifndef _CO_TCP_H
#define _CO_TCP_H

#include <cstdint>

#include <windows.h>
#include <winsock2.h>

#include "SIO.h"


class cO_tcp : public SIOOutbound
{
public:
    // SIOOutbound methods:
    virtual int  busyRead();
    virtual void write(char *ptr, int bytes);
    virtual void writeChar(char ch);
    virtual void onKey(bool, int, int, int) {}

    bool connect(SOCKET socket);
    bool connected();

    cO_tcp();
    virtual ~cO_tcp();
private:
    SOCKET so;
    char chInput;

    // network reader thread:-
    DWORD tcpReader();
    HANDLE thread;
    HANDLE go;
    bool reading;

    static DWORD __stdcall tcpWorker(void *pv);
};

#endif //_CO_TCP_H
