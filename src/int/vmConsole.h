#if !defined(CONSOLE_H)
#define CONSOLE_H

#include "SIO.h"
#include "sio_in.h"
#include "sio_out.h"

class Console : public SIO_In
{
public:
    Console(int addr, int ipt);     // Local machine console ctor
    Console();

    virtual ~Console();

    // SIOInbound implementation:
    virtual int  addr();
    virtual int  ipt();
    virtual int  inpIpt();
    virtual int  outIpt();
    virtual int  inp(int addr);
    virtual void out(int addr, int data);

    virtual void write(char *ptr, int bytes);
    virtual void writeChar(char ch);
    virtual void onKey(bool bDown, int nVirtKey, int lKeyData, int ch);
};

#endif // CONSOLE_H
