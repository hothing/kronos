#ifndef SIO_GEN_IN_H
#define SIO_GEN_IN_H

#include "sio_in.h"
#include "sio_out.h"

class cI : public SIO_In
{
public:
    // SIOInbound methods:
    int  addr();
    int  ipt();
    int  inpIpt();
    int  outIpt();

    int  inp(int addr);
    void out(int arrd, int data);

    cI(int addr, int ipt, SIO_Out *p);
protected:
    SIO_Out *po;
    int  sioAddr;
    int  sioIpt;
    bool inpIptEnabled;
    bool outIptEnabled;
    int  inChar;
private:
    void _read();
};

#endif
