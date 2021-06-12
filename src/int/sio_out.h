#ifndef SIO_OUT_H
#define SIO_OUT_H

enum { EMPTY = 512 }; // to allow 0x00 to pass through

class SIO_Out
{
  public:
//  Serial line IO VM side:-
    virtual int  busyRead() = 0;  // should return EMPTY if not ready
    virtual void write(char *ptr, int bytes) = 0;
    virtual void writeChar(char ch) = 0;
    virtual void onKey (bool bDown, int nVirtKey, int lKeyData, int ch) = 0;
    virtual ~SIO_Out() = 0;
};

#endif
