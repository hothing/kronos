#include <cstring>
#include <cstddef>
#include <cstdio>
#include <cstdint>

#include "cO_win32.h"

///////////////////////////////////////////////////////////////////////////////
// SIOOutbound:- Win32 console connection implementation

int cO_win32::busyRead()
{
    int ch = EMPTY;

    if (szVK[0] != 0)
    {
        ch = szVK[0];
        strcpy(szVK, &szVK[1]);
        return ch;
    }
    else if (!reading)
        SetEvent(go);

    return ch;
}


uint32_t __stdcall cO_win32::kbdWorker(void *pv)
{
    cO_win32 *co = (cO_win32 *)pv;
    return co->kbdReader();
}


void cO_win32::onKey(bool bDown, int nVirtKey, int lKeyData, int ch)
{
    uint32_t nWritten = 0;
    INPUT_RECORD Buffer;
    Buffer.EventType = KEY_EVENT;
    Buffer.Event.KeyEvent.bKeyDown = bDown;
    Buffer.Event.KeyEvent.wRepeatCount = uint16_t(lKeyData & 0xFFFF);
    Buffer.Event.KeyEvent.wVirtualKeyCode = uint16_t(nVirtKey);
    Buffer.Event.KeyEvent.wVirtualScanCode = uint16_t((lKeyData >> 16) & 0xFF);
    Buffer.Event.KeyEvent.uChar.AsciiChar = uint8_t(ch & 0xFF);
    Buffer.Event.KeyEvent.dwControlKeyState = 0;
    if (!::WriteConsoleInput(stdIn, &Buffer, 1, &nWritten) || nWritten < 1)
    {
        uint32_t dw = GetLastError();
        fprintf(stderr, "WriteConsoleInput: %d [%08X]\n", dw, dw);
    }
}


uint32_t WINAPI cO_win32::kbdReader()
{
    while (WaitForSingleObject(go, INFINITE) == WAIT_OBJECT_0)
    {
        INPUT_RECORD Buffer;
        uint32_t nRead = 0;

        reading = true;
        if (!ReadConsoleInput(stdIn, &Buffer, 1, &nRead) || nRead < 1)
        {
            uint32_t dw = GetLastError();
            fprintf(stderr, "ReadConsoleInput: %d [%08X]\n", dw, dw);
        }
        else if ((Buffer.EventType & KEY_EVENT) != 0 && Buffer.Event.KeyEvent.bKeyDown)
        {
            if (!decode(Buffer.Event.KeyEvent.wVirtualKeyCode))
            {
                szVK[0] = char(Buffer.Event.KeyEvent.uChar.AsciiChar & 0xFF);
//              trace("ch=%02x\n", szVK[0]);
                szVK[1] = 0;
            }
            else
            {
//              trace("ch=%02x%02x\n", szVK[0],szVK[1]);
            }
        }
        reading = false;
    }
    return 0;
}


cO_win32::cO_win32() :
    stdIn(0),
    thread(nullptr),
    go(nullptr),
    reading(false)
{
    uint32_t id;
    go = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (go == nullptr)
    {
        delete this;
        return;
    }

//  trace("creating kbdReader\n");
    thread = CreateThread(NULL, 4096, kbdWorker, (void *)this, 0, &id);
    if (thread == nullptr)
    {
        delete this;
        return;
    }

    if (SetThreadPriority(thread, THREAD_PRIORITY_TIME_CRITICAL) == 0)
    {
        delete this;
        return;
    }

    memset(szVK, 0, sizeof szVK);
    stdIn  = ::GetStdHandle(STD_INPUT_HANDLE);

    uint32_t dwMode = 0;
    GetConsoleMode(stdIn, &dwMode);
    dwMode &= ~(ENABLE_PROCESSED_INPUT|ENABLE_LINE_INPUT|
                ENABLE_ECHO_INPUT|ENABLE_WINDOW_INPUT|ENABLE_MOUSE_INPUT);

    SetConsoleMode(stdIn, dwMode);
}


cO_win32::~cO_win32()
{
    TerminateThread(thread, 0);
    CloseHandle(thread);
    CloseHandle(go);
}


int cO_win32::decode(uint16_t vkChar)
{
    switch (vkChar)
    {
        case VK_ESCAPE: strcpy(szVK, "\033\033");   break;
        case VK_UP:     strcpy(szVK, "\033A");      break;
        case VK_DOWN:   strcpy(szVK, "\033B");      break;
        case VK_LEFT:   strcpy(szVK, "\033D");      break;
        case VK_RIGHT:  strcpy(szVK, "\033C");      break;
        case VK_INSERT: strcpy(szVK, "\233R");      break;
        case VK_DELETE: strcpy(szVK, "\233S");      break;
        case VK_HOME:   strcpy(szVK, "\233G");      break;
        case VK_END:    strcpy(szVK, "\233O");      break;
        case VK_PRIOR:  strcpy(szVK, "\033?\156");  break;
        case VK_NEXT:   strcpy(szVK, "\033?\115");  break;
        case VK_F1:     strcpy(szVK, "\033P");      break;
        case VK_F2:     strcpy(szVK, "\033Q");      break;
        case VK_F3:     strcpy(szVK, "\033R");      break;
        case VK_F4:     strcpy(szVK, "\033S");      break;
        case VK_F5:     strcpy(szVK, "\033?\160");  break;
        case VK_F6:     strcpy(szVK, "\033?\161");  break;
        case VK_F7:     strcpy(szVK, "\033?\162");  break;
        case VK_F8:     strcpy(szVK, "\033?\163");  break;
        case VK_F9:     strcpy(szVK, "\033?\164");  break;
        case VK_F10:    strcpy(szVK, "\033?\165");  break;

        default:
            return false;
    }
    return true;
}

void cO_win32::write(char *ptr, int bytes) { d.write(ptr, bytes); }
void cO_win32::writeChar(char ch) { d.writeChar(ch); }
