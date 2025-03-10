#include <cstring>
#include <cassert>
//#include <winuser.h>

#include "Memory.h"
#include "IGD480.h"

#include "resource.h"

#ifndef GWL_USERDATA
#define GWL_USERDATA (-21)
#endif

IGD480::IGD480(MEMORY* m, SioMouse* sioMouse, Console* con) :
    mem(*m), mouse(*sioMouse), console(*con),
    thread(nullptr),
    bRun(true),
    dwShift(0),
    dwLock(0xFFFFFFFF),
    nCursor(0),
    hWnd(nullptr),
    hStaticWnd(nullptr),
    lResult(0),
    pBits(nullptr),
    mdc(nullptr),
    bdc(nullptr),
    hBitmap(nullptr),
    hbmpScreen(nullptr),
    mx(480/2),
    my(360/2)
{
    uint32_t id = 0;
    thread = ::CreateThread(nullptr, 0, rawDisplayThread, (void*)this, 0, &id);
    ::SetThreadPriority(thread, THREAD_PRIORITY_BELOW_NORMAL);
    // THREAD_PRIORITY_LOWEST?
    // THREAD_PRIORITY_IDLE
}


IGD480::~IGD480()
{
    shutdown();
    if (hBitmap != nullptr)
    {
        ::DeleteObject(hBitmap);    hBitmap = nullptr;
        ::DeleteObject(hbmpScreen); hbmpScreen = nullptr;
        ::DeleteDC(mdc);            mdc = nullptr;
        ::DeleteDC(bdc);            bdc = nullptr;
    }
    pBits = nullptr;
}



uint32_t IGD480::displayThread()
{
    while (bRun)
    {
        ::Sleep(20); // ~50 frames per second
        uint32_t shift = mem.data[IGD480base + 0x00] & ~0x1;
        uint32_t lock = mem.data[IGD480base + 0x0F];
        if (lock != dwLock)
        {
//          trace("lock=%d\n", lock);
            dwLock = lock;
            if (hWnd == nullptr)
            {
                createWindow();
                createBitmap();
            }
            if (hWnd != nullptr)
            {
                ::ShowWindow(hWnd, dwLock ? SW_SHOW : SW_HIDE);
                if (dwLock)
                {
                    ::SetForegroundWindow(hWnd);
                    ::BringWindowToTop(hWnd);
                    ::SetWindowPos(hWnd, HWND_TOP, -1, -1, -1, -1, SWP_NOMOVE|SWP_NOSIZE);
                }
                if (dwLock) // all Kronos programs start with cursor in the middle
                {
                    mx = 480 / 2;
                    my = 360 / 2;
                }
            }
        }
        mem.data[IGD480base + 0x00] |=  0x01;   // frame sync
        if (shift != dwShift)
        {
            dwShift = shift;
            //trace("dwShift=%08X\n", dwShift);
        }
//      mem.data[IGD480base + 0x20] &= ~0x01;   // line  sync (not necessary, faster w/o it)
        refresh();
        ::Sleep(100);
        mem.data[IGD480base + 0x00] &= ~0x01;   // frame sync
//      mem.data[IGD480base + 0x20] |=  0x01;   // line  sync (not necessary, faster w/o it)
    }
    return 0;
}


void IGD480::shutdown()
{
    bRun = false;
    if (thread != nullptr)
    {
        ::WaitForSingleObject(thread, 1000);
        ::CloseHandle(thread);  thread = nullptr;
    }
    if (hWnd != nullptr)
    {
        ::DestroyWindow(hWnd);  hWnd = nullptr;
        ::Sleep(200);
    }
}



uint32_t IGD480::rawDisplayThread(void* pvThis)
{
    IGD480* pThis = (IGD480*)pvThis;
    return pThis->displayThread();
}




/////////////////////////////////////////////////////////////////


void IGD480::onPaint()
{
}

//CONST trans = ARRAY OF BITSET{{},{2},{1},{1..2},{0},{0,2},{0..1},{0..2}};
int trans[8] = { 0x0, 0x4, 0x2, 0x6, 0x1, 0x5, 0x3, 0x7 };
//      assert(trans[trans[i]] == i);


bool IGD480::wndProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_KEYDOWN: 
//          trace("WM_KEYDOWN=%02x\n", wParam & 0xFF);
            if ((VK_PRIOR <= wParam && wParam <= VK_HELP)
            ||  (VK_F1 <= wParam && wParam <= VK_F12))
            {
                // we won't receive WM_CHAR on those
                console.onKey(true, wParam, 0x00000001, wParam & 0xFF);
            }
            break;

        case WM_KEYUP: 
            console.onKey(false, wParam, lParam, wParam & 0xFF);
//          trace("WM_KEYUP=%02x\n", wParam & 0xFF);
            break;

        case WM_CHAR: 
            console.onKey(true, wParam, lParam, wParam & 0xFF);
//          trace("WM_CHAR=%02x\n", wParam & 0xFF);
            return true;

        case WM_SYSCOMMAND: 
        {
            uint32_t uCommand = wParam & ~0xF;
            if (uCommand == SC_MOVE
            ||  uCommand == SC_MINIMIZE
            ||  uCommand == SC_RESTORE)
                break; // continue processing
            else
                return true; // prohib others 
        }

        case WM_ERASEBKGND:
            return true;


        case WM_GETICON:
            lResult = (LRESULT)::LoadIcon(::GetModuleHandle(nullptr), 
                                          MAKEINTRESOURCE(IDI_KRONOS));
            return true;

        case WM_NCMOUSEMOVE:
            if (nCursor > 0)
            {
                ::ShowCursor(true);
                nCursor--;
            }
            break;

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MOUSEMOVE:
        {
            uint32_t dwKeys = wParam;      // key flags 
            int xPos = LOWORD(lParam);  // horizontal position of cursor 
            int yPos = HIWORD(lParam);  // vertical position of cursor 
            RECT rc;
            ::GetClientRect(hWnd, &rc);
            POINT pt;
            pt.x = xPos;
            pt.y = yPos;
//          trace("%d %d\n", xPos, yPos);
            if (::PtInRect(&rc, pt))
            {
                if (nCursor == 0)
                {
                    ::ShowCursor(false);
                    nCursor++;
                }
            }
            else
            {
            }
            uint32_t dwState = 0;
            dwState |= (dwKeys & MK_LBUTTON) ? 0x1 : 0x0;
            dwState |= (dwKeys & MK_RBUTTON) ? 0x2 : 0x0;
            dwState |= (dwKeys & MK_MBUTTON) ? 0x4 : 0x0;
            dwState ^= 0x7;
            yPos = rc.bottom - yPos - 1;
//          trace("%d %d was %d %d\n", xPos, yPos, mx, my);
            mouse.changeState(trans[dwState], xPos - mx, yPos - my);
            mx = xPos;
            my = yPos;
            return true;
        }
    }
    return false;
}

LRESULT IGD480::rawWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    bool bProcessed = false;
    IGD480* pThis = nullptr;
    if (msg == WM_CREATE)
    {
        CREATESTRUCT* pCS = (CREATESTRUCT*)lParam;
        pThis = (IGD480*)pCS->lpCreateParams;
        ::SetWindowLong(hWnd, GWL_USERDATA, (long)pCS->lpCreateParams);
        pThis->lResult = 0;
        pThis->hWnd = hWnd;
        bProcessed = pThis->wndProc(msg, wParam, lParam);
    }
    else
    {
        pThis = (IGD480*)GetWindowLong(hWnd, GWL_USERDATA);
        if (pThis != nullptr)
        {
            pThis->lResult = 0;
            bProcessed = pThis->wndProc(msg, wParam, lParam);
        }
    }
    if (!bProcessed)
        return ::DefWindowProc(hWnd, msg, wParam, lParam);
    else
        return pThis->lResult;
}


void IGD480::createWindow()
{
  WNDCLASS cls;
  ATOM a;
    cls.style       = CS_VREDRAW|CS_HREDRAW;
    cls.style      &= ~CS_DBLCLKS;
    cls.lpfnWndProc = rawWndProc;
    cls.cbClsExtra = 0;
    cls.cbWndExtra = 0;
    cls.hInstance = ::GetModuleHandle(nullptr);
    cls.hIcon = ::LoadIcon(cls.hInstance, MAKEINTRESOURCE(IDI_KRONOS));
    cls.hCursor = ::LoadCursor(nullptr, IDC_CROSS); 
    cls.hbrBackground = (HBRUSH)::GetStockObject(NULL_BRUSH);
    cls.lpszMenuName = nullptr;
    cls.lpszClassName = "KronosIGD480";
    uint32_t dwStyle = WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX;
    RECT rc = {0, 0, 480, 360};
  AdjustWindowRectEx(&rc, dwStyle, /*menu:*/false, WS_EX_APPWINDOW);
  a = ::RegisterClass(&cls);
  hWnd = ::CreateWindowEx(WS_EX_APPWINDOW,
                          cls.lpszClassName, 
                          "Kronos Graphics",
                          dwStyle,
			  CW_USEDEFAULT, 
			  CW_USEDEFAULT,
                          rc.right - rc.left, rc.bottom - rc.top,
			  nullptr, 
			  nullptr, 
                          ::GetModuleHandle(nullptr),
                          this);

    HMENU hMenu = ::GetSystemMenu(hWnd, false);
    ::DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
    ::DeleteMenu(hMenu, SC_MAXIMIZE, MF_BYCOMMAND);
    ::DeleteMenu(hMenu, SC_SIZE, MF_BYCOMMAND);
    ::DeleteMenu(hMenu, GetMenuItemCount(hMenu)-1, MF_BYPOSITION);

  hStaticWnd = ::CreateWindow("STATIC", 
			      nullptr, 
			      WS_CHILD|WS_VISIBLE|SS_BITMAP, 
			      0, 0, 480, 360, 
			      hWnd, 
			      nullptr, 
			      ::GetModuleHandle(nullptr), 
			      nullptr);
    mdc = ::CreateCompatibleDC(nullptr);
    bdc = ::CreateCompatibleDC(nullptr);
}


void IGD480::createBitmap()
{
    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof bmi);
    BITMAPINFOHEADER& bih = bmi.bmiHeader;
    bih.biSize = sizeof bmi;
    bih.biWidth = 480;
    bih.biHeight = 360;
    bih.biPlanes = 1;
    bih.biBitCount = 24;
    bih.biCompression = 0;
    bih.biSizeImage = (bih.biWidth * bih.biHeight * bih.biBitCount * bih.biPlanes) / 8;
    bih.biXPelsPerMeter = 0;
    bih.biYPelsPerMeter = 0;
    bih.biClrUsed = 0;
    bih.biClrImportant = 0;
    ::GdiFlush();
    hBitmap = ::CreateDIBSection(mdc, (const BITMAPINFO*)&bmi, 0, (void**)&pBits, NULL, 0);

    HDC hDC = ::GetDC(hStaticWnd);
    hbmpScreen = ::CreateCompatibleBitmap(hDC, 480, 360);
    ::ReleaseDC(hStaticWnd, hDC);
    ::SendMessage(hStaticWnd, STM_SETIMAGE, IMAGE_BITMAP, long(hbmpScreen));
}

const int ptrans[] =
{ 0xF,0x7,0xB,0x3,0xD,0x5,0x9,0x1,0xE,0x6,0xA,0x2,0xC,0x4,0x8,0x0 };



void IGD480::copyBitmap()
{
    // dwShift:
    // shy0 = 151+512; -- 151 + 360 == 511
    // sh := ((shy0-screen.ldcy)*200h+screen.ldcx)*400h;
    int ldcy = ((dwShift / 0x400) / 0x200) % 512;
    int ldcx = ((dwShift / 0x400) % 0x200) % 512;

    uint32_t* pal = (uint32_t*)(byte*)&mem[IGD480base + 0x10];
    uint8_t palette[16][3];
    for (int i = 0; i < 16; i++)
    {
        assert(ptrans[ptrans[i]] == i);
        int c = pal[i];
        c = pal[i];      int r = ptrans[c % 16] / 4;
        c = c / 16;      int g = ptrans[c % 16] / 4;
        c = c / 16;      int b = ptrans[c % 16] / 4;
        r = (r == 0 ? 0 : (r+1) * 64 - 1);
        g = (g == 0 ? 0 : (g+1) * 64 - 1);
        b = (b == 0 ? 0 : (b+1) * 64 - 1);
        palette[i][0] = (uint8_t)r;
        palette[i][1] = (uint8_t)g;
        palette[i][2] = (uint8_t)b;
    }
    const int wpl = 512 / 32;

    for (int y = 0; y < 360; y++)
    {
        int x = 0; // should also be refrelecting ldcx
        for (int i = 0; i < 480 / 32; i++)
        {
            int Y = (ldcy + y) % 512;
            uint32_t w0 = mem[IGD480bitmap + (Y + 0*512) * wpl + ldcx / 32 + i];
            uint32_t w1 = mem[IGD480bitmap + (Y + 1*512) * wpl + ldcx / 32 + i];
            uint32_t w2 = mem[IGD480bitmap + (Y + 2*512) * wpl + ldcx / 32 + i];
            uint32_t w3 = mem[IGD480bitmap + (Y + 3*512) * wpl + ldcx / 32 + i];
            for (int j = 0; j < 32; j++)
            {
                int n = 
                    ((w0 << 0) & 0x1) |
                    ((w1 << 1) & 0x2) |
                    ((w2 << 2) & 0x4) |
                    ((w3 << 3) & 0x8);
                pBits[((359-y)*480 + x) * 3 + 0] = palette[n][2]; // B
                pBits[((359-y)*480 + x) * 3 + 1] = palette[n][1]; // G
                pBits[((359-y)*480 + x) * 3 + 2] = palette[n][0]; // R
                x++;
                assert(x <= 480);
                w0 >>= 1;
                w1 >>= 1;
                w2 >>= 1;
                w3 >>= 1;
            }
        }
    }
}


void IGD480::refresh()
{
    MSG msg;
    while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
    if (!::IsIconic(hWnd) && ::IsWindowVisible(hWnd))
    {
        //::GdiFlush(); ??
//      trace("BitBlt\n");
        copyBitmap();
        HGDIOBJ hMdcSafeBmp = ::SelectObject(mdc, hBitmap);
        HGDIOBJ hBdcSafeBmp = ::SelectObject(bdc, hbmpScreen);
        int nOldStretchMode = ::SetStretchBltMode(bdc, HALFTONE);
        POINT pt = {0,0};
        BOOL bSetOrigin = ::SetBrushOrgEx(bdc, 0, 0, &pt);
        (void)bSetOrigin; // unused
        BOOL r = ::BitBlt(bdc, 0, 0, 480, 360, mdc, 0, 0, SRCCOPY);
        (void)r; // unused
        ::SetStretchBltMode(bdc, nOldStretchMode);
        ::SelectObject(bdc, hBdcSafeBmp);
        ::SelectObject(mdc, hMdcSafeBmp);
        ::InvalidateRect(hStaticWnd, nullptr, false);
        ::InvalidateRect(hWnd, nullptr, false);
    }
}

