#include <cstddef>
#include <cstdint>

#include "sio_gen_in.h"


cI::cI(int a, int i, SIO_Out *p) :
    po(p),
    sioAddr(a),
    sioIpt(i),
    inpIptEnabled(false),
    outIptEnabled(false),
    inChar(EMPTY)
{
}


int cI::inp(int addr)
{
    switch (addr & 0x0003)
    {
    case 0:
        if (inChar == EMPTY)
            inChar = po->busyRead();

        return (inpIptEnabled ? 0100 : 0) | (inChar != EMPTY ? 0200 : 0);
    case 1:
        {
            if (inChar == EMPTY)
                inChar = po->busyRead();

            int data = inChar == EMPTY ? 0 : inChar;
            inChar = EMPTY;
//          trace("inp(0x%04X)=%02X\n", addr, data & 0xFF);
            return data & 0xFF;
        }
    case 2:
            return 0200 | (outIptEnabled ? 0100 : 0);
    case 3:
        return 0; // return outputData();
    }
    return 0;
}


void cI::out(int addr, int value)
{
    switch (addr & 0x0003)
    {
    case 0: inpIptEnabled = (value & 0100) != 0;    break;
    case 1:                                         break;
    case 2: outIptEnabled = (value & 0100) != 0;    break;
    case 3: po->writeChar((uint8_t)value);             break;
    }
}

int cI::inpIpt()
{
    if (inChar == EMPTY)
        inChar = po->busyRead();
    return inpIptEnabled && inChar != EMPTY;
}

int cI::outIpt()
{
    return outIptEnabled;
}

int cI::ipt()
{
    return sioIpt;
}

int cI::addr()
{
    return sioAddr;
}
