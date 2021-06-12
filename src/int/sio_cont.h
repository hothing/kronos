#ifndef SIO_GEN_IN_H
#define SIO_GEN_IN_H

#include "SIO.h"

class SIO_Controller
{
public:
    SIO_Controller();
    virtual ~SIO_Controller();

    void addSIO(SIO *s);

    SIO *inpReady();
    SIO *outReady();

    SIO *find(int ioAddr);

private:
    enum { max_sio = 10 };
    SIO *rgsio[max_sio];
    int  N;
    int  lastIpted;
};

#endif
