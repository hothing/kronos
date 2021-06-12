#if !defined(SIO_H)
#define SIO_H

#include "sio_in.h"
#include "sio_out.h"

class SIO : public SIO_In, public SIO_Out
{
};

#endif //SIO_H
