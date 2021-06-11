#ifndef VM_H
#define VM_H

#include <cstdint>

#include "Memory.h"
#include "IGD480.h"
#include "Disks.h"
#include "SIO.h"
#include "siomouse.h"
#include "vmConsole.h"

enum {  AStackSize = 15,
        Nil = 0x7FFFFF80,
        ExternalBit = 31
     };

class VM
{
public:
    VM(int nMemorySizeBytes, SioMouse*, Console*);
    virtual ~VM();
    void Run();

    bool (*DiskRead)    (int diskno, int block, uint8_t* adr, int len);
    bool (*DiskWrite)   (int diskno, int block, uint8_t* adr, int len);

    MEMORY mem;
    IGD480 igd;
    DISKS Disks;
    int DiskOperation(int op, int dsk, int sec, int adr, int len);

    SIOs sios;

    void setConsole(SIO *ps);
    int  busyRead();
    void printf(const char* fmt, ...);

private:
    int PC;
    int PCs;
    int IR;
    int F;
    int G;
    int L;
    int S;
    int H;
    int P;
    int M;
    int Ipt;
    int AStack[AStackSize];
    int sp;
    uint8_t* code;

    bool bTimer; // 20 msec interrupt source

    SIO *con;

    uint8_t* GetCode(int f);

    inline void Push(int w);
    inline int  Pop();
    inline int  Next();
    inline int  Next2();
    inline int  Next4();

    void Mark(int x, bool Extern);
    void SaveAStack();
    void SaveRegisters();
    void RestoreAStack();
    void RestoreRegisters();
    void Trap(int no);
    void Transfer(int p_to, int p_from);

    void IO(int no);
    void BMG(int no);
    void FPU();
    void Quote(int op);

    void bitBlt(uint32_t* dst, int dofs, uint32_t* src, int sofs, int bits);
    void BitBlt(uint32_t  dst, int dofs, uint32_t  src, int sofs, int bits);
    void BitMove(int t, int t_o, int f, int f_o, int sz);
    uint32_t BBU(int adr, int i, int sz);
    void  BBP(int adr, int i, int sz, int j);

    void _gbblt(int mode, void* des, int dofs, void* sou, int sofs, int nobits);
    void dot(int mode, Bitmap* bmp, int x, int y);
    void hline(int mode, Bitmap* bmp, int x, int y, int x1);
    void drawline(int mode, Bitmap* bmp, int x, int y, int x1, int y1);
    void drawclipedline(int mode, Bitmap* bmp, int x0, int y0, int x1, int y1, int w, int h);
    int  inrect(int x, int y, int w, int h);
    void vline(int mode, Bitmap* bmp, int x, int y, int len);
    void gbblt(int mode, int des, int dofs, int sou, int sofs, int nobits);
    void dch(int mode, Bitmap* bmp, int x, int y, Font* font, int ch);
    int  clip(Clip* ctx, int w, int h);
    void line(int mode, Bitmap* bmp, int x, int y, int x1, int y1);
    void circle(int mode, Bitmap* bmp, Circle* ctx, int x, int y);
    void arc(int mode, Bitmap* bmp, ArcCtx* ctx);
    void trif(TriangleFilled*);
    void circlef(CircleFilled*);

    void ShowRegisters();
    bool DebugMonitor(int& a);
    void digits(int& a, char ch);

    int  diskno;

    bool bDebug;
};

#endif //VM_H
