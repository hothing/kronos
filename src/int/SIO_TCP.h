#ifndef _SIO_TCP_H
#define _SIO_TCP_H

#include <cstdint>
#include <windows.h>
#include <winsock2.h>

#include "SIO.h"

class SioTcp : public SIO
{
public:
    SioTcp(int addr, int ipt);
    virtual ~SioTcp();

    // SIOInbound implementation:
    virtual int  addr();
    virtual int  ipt();
    virtual int  inpIpt();
    virtual int  outIpt();
    virtual int  inp(int addr);
    virtual void out(int addr, int data);

    // SIOOutbound implementation:
    virtual int  busyRead();
    virtual void write(char *ptr, int bytes);
    virtual void writeChar(char ch);
    virtual void onKey(bool, int, int, int) {}

    bool connect(SOCKET socket);
    bool connected();

private:
    SIOInbound *i;
    SIOOutbound *o;
};


class SioTcps
{
public:
    SioTcps(uint16_t port);
    virtual ~SioTcps();

    int addClient(SioTcp *p);
    bool start();

    DWORD Worker();

private:
    enum { max_tcp = 9 };
    SioTcp *rgtcp[max_tcp];
    int N;

    uint16_t port;
    HANDLE thread;

    SioTcp *find();
    void serve(uint32_t so);
};

#endif //_SIO_TCP_H
