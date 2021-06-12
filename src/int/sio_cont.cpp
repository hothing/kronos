#include "sio_cont.h"

SIO_Controller::SIO_Controller() : N(0), lastIpted(0)
{
}

SIO_Controller::~SIO_Controller()
{
/*
    for (int i = 0; i < N; ++i)
    {
        delete rgsio[i];
        rgsio[i] = null;
    }
    N = 0;
*/
}


void SIO_Controller::addSIO(SIO *s)
{
    if (N < max_sio)
        rgsio[N++] = s;
}


SIO * SIO_Controller::inpReady()
{
    lastIpted = (lastIpted + 1) % N;

    if (rgsio[lastIpted]->inpIpt())
        return rgsio[lastIpted];

    return nullptr;
}


SIO *SIO_Controller::outReady()
{
    if (rgsio[lastIpted]->outIpt())
        return rgsio[lastIpted];

    return nullptr;
}


SIO *SIO_Controller::find(int addr)
{
    for (int i = 0; i < N; ++i)
    {
        if (rgsio[i]->addr() == addr)
            return rgsio[i];
    }
    return nullptr;
}
