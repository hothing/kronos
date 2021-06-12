///////////////////////////////////////////////////////////////////
// IGD480.h
#ifndef _IGD480_H
#define _IGD480_H

#include <cstdint>
#include <windows.h>

#include "vmConsole.h"
#include "Memory.h"
#include "sio_mouse.h"

struct Bitmap
{
    int w;
    int h;
    int wpl;
    int base;
    int pattern;
    int mask;
    int layers[8];
};


struct Font
{
    int w;
    int h;
    int base;
    int rfe;
};


struct Clip
{
    int x0;
    int y0;
    int x1;
    int y1;
};


struct ArcCtx // Arc is taken by GDI
{
    int X;
    int Y;
    int x0;
    int y0;
    int x1;
    int y1;
    int r;
    int x;
    int y;
    int co;
    int xy0;
    int xx0;
    int xy1;
    int xx1;
    int yx0;
    int yy0;
    int yx1;
    int yy1;
    int Case;
};


struct Circle
{
    int x;
    int y;
    int co;
};


struct CircleFilled
{
    int x;
    int y;
    int co;
    int xn;
    int yn;
    int Do;
};


struct TriangleFilled
{
    int x;
    int y;
    int co;
    int xn;
    int yn;
          
    int Dx;
    int Dy;
    int dx;
    int dy;
    int xl;  
    int Gx;
    int Case;
};


enum // mode
{
    mode_rep = 0, // destinator := source
    mode_or  = 1, // destinator := destinator OR  source
    mode_xor = 2, // destinator := destinator XOR source
    mode_bic = 3  // destinator := destinator AND NOT source
};


class IGD480
{
public:
    IGD480(MEMORY* mem, SioMouse*, Console*);
    virtual ~IGD480();
    void shutdown();

private:
    MEMORY&   mem;
    SioMouse& mouse;
    Console&  console;

    void* thread;
    bool  bRun;

    uint32_t dwShift;
    uint32_t dwLock;
    int   nCursor;

    HWND  hWnd;
    HWND  hStaticWnd;
    long  lResult;
    BYTE* pBits;
    HDC   mdc;
    HDC   bdc;
    HBITMAP hBitmap;
    HBITMAP hbmpScreen;
    int   mx;   // mouse x
    int   my;   // mouse y

    void  refresh();
    uint32_t displayThread();
    bool  wndProc(UINT msg, WPARAM wParam, LPARAM lParam);
    void  onPaint();
    void  createWindow();
    void  createBitmap();
    void  copyBitmap();

    static
    uint32_t __stdcall rawDisplayThread(void*);
    static
    LRESULT __stdcall rawWndProc(HWND, UINT, WPARAM, LPARAM);
};

#endif //_IGD480_H
