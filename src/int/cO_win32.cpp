#include "cO_win32.h"

///////////////////////////////////////////////////////////////////////////////
// SIOOutbound:- Win32 console connection implementation

int cO_win32::busyRead()
{
    int ch = EMPTY;

    return ch;
}

void cO_win32::onKey(bool bDown, int nVirtKey, int lKeyData, int ch)
{

}

cO_win32::cO_win32() :
    reading(false)
{

}


cO_win32::~cO_win32()
{

}


int cO_win32::decode(uint16_t vkChar)
{
    return true;
}

void cO_win32::write(char *ptr, int bytes) { d.write(ptr, bytes); }
void cO_win32::writeChar(char ch) { d.writeChar(ch); }
