#ifndef _SIO_MOUSE_H
#define _SIO_MOUSE_H

#include <cstdint>

#include "SIO.h"

class SioMouse : public SIO
{
public:
    SioMouse(int addr, int ipt);
    virtual ~SioMouse();

    // SIOInbound implementation:
    int  addr();
    int  ipt();
    int  inpIpt();
    int  outIpt();
    int  inp(int addr);
    void out(int addr, int data);

    // SIOOutbound implementation:
    virtual int  busyRead();
    virtual void write(char *ptr, int bytes);
    virtual void writeChar(char ch);
    virtual void onKey(bool, int, int, int) { }

    // IGD480 calls changeState:
    void changeState(uint32_t dwKeys, int dx, int dy);
private:
    long nIn;
    long nOut;
    uint8_t buf[5*1024];
    cI*  i;
};

#endif //_SIO_MOUSE_H
