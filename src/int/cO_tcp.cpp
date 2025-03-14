#include <cstddef>
#include <windows.h>

#include "cO_tcp.h"


int cO_tcp::busyRead()
{
    if (!connected()) return EMPTY;

    int ch = EMPTY;

    if (chInput != 0)
    {
        ch = chInput;
        chInput = 0;
    }
    else if (!reading) SetEvent(go);

    return ch;
}


DWORD __stdcall cO_tcp::tcpWorker(void *pv)
{
    cO_tcp *p = (cO_tcp *)pv;
    return p->tcpReader();
}


DWORD cO_tcp::tcpReader()
{
    while (WaitForSingleObject(go, INFINITE) == WAIT_OBJECT_0)
    {
        char ch = 0;

        reading = true;

        int err = recv(so, &ch, 1, 0);

        if (err == SOCKET_ERROR) { so = INVALID_SOCKET; }
        else { chInput = ch; }

        reading = false;
    }
    return 0;
}


void cO_tcp::write(char *ptr, int bytes)
{
    if (!connected()) return;
    int err = send(so, ptr, bytes, 0);
    if (err == SOCKET_ERROR) { so = INVALID_SOCKET; }
}

void cO_tcp::writeChar(char ch)
{
    write(&ch, 1);
}

bool cO_tcp::connect(SOCKET s)
{
    if (so == INVALID_SOCKET)
        so = s;

    return so == s;
}

bool cO_tcp::connected()
{
    return so != INVALID_SOCKET;
}

cO_tcp::cO_tcp() : so(INVALID_SOCKET), thread(0), go(0), reading(0)
{
    DWORD id;
    go = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (go == NULL)
    {
        delete this;
        return;
    }

    thread = CreateThread(NULL, 4096, tcpWorker, (void *)this, 0, &id);
    if (thread == NULL)
    {
        delete this;
        return;
    }

    if (SetThreadPriority(thread, THREAD_PRIORITY_TIME_CRITICAL) == 0)
    {
        delete this;
        return;
    }
}   


cO_tcp::~cO_tcp()
{
    if (thread != NULL) TerminateThread(thread, 0);
    if (thread != NULL) CloseHandle(thread);
    if (go != NULL) CloseHandle(go);
}
