#ifndef SIO_IN_H
#define SIO_IN_H

class SIO_In
{
  public:
// Serial line IO Kronos side:-
    virtual int  addr() = 0;    // Q-BUS IO base addr, one serial line uses 4 consecutive shorts
                                // i.e. [0177560..0177570)
                                // Note that Kronos IO uses halved values:
                                // 0177560 = 0xFF70 = 0x7FB8 * 2
                                // Kronos uses 0x7FB8..0x7FBB (or 0x0FB8,,0x0FBB, since high bits of
                                // IO bus are const 0x7000 :-)

    virtual int  ipt() = 0;     // We use two consecutive ipt numbers for serial line
                                // i.e. for default Q-BUS serial line it would be 0x0C for input and
                                // 0x0D for output

    virtual int  inpIpt() = 0;  // interrupt request from input line
    virtual int  outIpt() = 0;  // interrupt request from output line

    virtual int  inp(int addr) = 0;
    virtual void out (int arrd, int data) = 0;

    virtual ~SIO_In() = 0;
};

#endif
