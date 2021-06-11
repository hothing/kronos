#if !defined(CO_WIN32_H)
#define CO_WIN32_H

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
    char  szVK[8];
    bool   reading;

    int  decode(uint16_t vkChar);
};
#endif
