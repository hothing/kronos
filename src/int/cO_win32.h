#ifndef _CO_WIN32_H
#define _CO_WIN32_H

#include <windows.h>
#include <cstdint>

#include "SIO.h"
#include "cO_win32_display.h"

class cO_win32 : public SIOOutbound
{
public:
    // SIOOutbound methods:
    virtual int  busyRead();
    virtual void write(char *ptr, int bytes);
    virtual void writeChar(char ch);
    virtual void onKey(bool bDown, int nVirtKey, int lKeyData, int ch);

    cO_win32();
    virtual ~cO_win32();

private:
    cO_win32_display d;
    HANDLE stdIn;
    char  szVK[8];

    // keyboard stream:-
    uint32_t WINAPI kbdReader();
    HANDLE thread;
    HANDLE go;
    bool   reading;

    int  decode(uint16_t vkChar);

    static uint32_t __stdcall kbdWorker(void *pv);
};

#endif //_CO_WIN32_H
