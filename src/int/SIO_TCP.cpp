#include <cstddef>
#include <cstdint>
#include <cstring>

#include <winsock2.h>
#include "SIO_TCP.h"
#include "cO_tcp.h"

///////////////////////////////////////////////////////////////////////////////
// SioTcp.

SioTcp::SioTcp(int addr, int ipt) : i(0), o(0)
{
    o = new cO_tcp;
    i = new cI(addr, ipt, o);
    if (o == nullptr || i == nullptr)
        delete this;
}

SioTcp::~SioTcp()
{
    if (i != nullptr)
    {
        delete i;
        i = nullptr;
    }
    if (o != nullptr)
    {
        delete o;
        o = nullptr;
    }
}

int  SioTcp::addr() { return i->addr(); }
int  SioTcp::ipt() { return i->ipt(); }
int  SioTcp::inpIpt() { return i->inpIpt(); }
int  SioTcp::outIpt() { return i->outIpt(); }
int  SioTcp::inp(int addr) { return i->inp(addr); }
void SioTcp::out(int addr, int data) { i->out(addr, data); }

int  SioTcp::busyRead() { return o->busyRead(); }
void SioTcp::write(char *ptr, int bytes) { o->write(ptr, bytes); }
void SioTcp::writeChar(char ch) { o->writeChar(ch); }

// Ugly, need to do something about it:-
bool SioTcp::connect(SOCKET so) { return ((cO_tcp*)o)->connect(so); }
bool SioTcp::connected() { return ((cO_tcp*)o)->connected(); }


///////////////////////////////////////////////////////////////////////////////
// SioTcps - server side


SioTcps::SioTcps(uint16_t p) : N(0), port(p)
{
}

SioTcps::~SioTcps()
{
    TerminateThread(thread, 0);
}

int SioTcps::addClient(SioTcp *p)
{
    if (N >= max_tcp)
        return 0;

    rgtcp[N++] = p;

    return N;
}


void SioTcps::serve(uint32_t so)
{
    SioTcp *tcp = find();

    if (tcp == NULL)
    {
        const char *msg =
            "Active connection limit has been reached.\r\n"
            "Please try again later.\r\n"
            "Thank you, Kronos Group.\r\n";

        send(so, msg, strlen(msg), 0);
        Sleep(2000);
        closesocket(so);
    }
    else
    {
        tcp->connect(so);
    }
}


DWORD SioTcps::Worker(void)
{
    SOCKET so = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (so == INVALID_SOCKET)
    {
        DWORD dw = WSAGetLastError();
        //trace("socket: %d [%08X]\n", dw, dw);
        return dw;
    }

  SOCKADDR_IN sin;
  DWORD dw;
    sin.sin_family = AF_INET;
    sin.sin_port   = htons(port);
    sin.sin_addr.S_un.S_addr = 0;

    if (bind(so, (SOCKADDR *)&sin, sizeof(sin)) != 0)
    {
        dw = WSAGetLastError();
        //trace("bind: %d [%08X]\n", dw, dw);
        return dw;
    }

    if (listen(so, SOMAXCONN) != 0)
    {
        dw = WSAGetLastError();
        //trace("listen: %d [%08X]\n", dw, dw);
        return dw;
    }

    do {
        //trace("Kronos server is accepting connections...\n");
        SOCKADDR_IN sinClient;
        int sinLen = sizeof(sinClient);
        DWORD soClient = accept(so, (SOCKADDR *)&sinClient, &sinLen);

        if (soClient != INVALID_SOCKET)
        {
            //trace("Connection from %d.%d.%d.%d\n",
            //    sinClient.sin_addr.S_un.S_un_b.s_b1, sinClient.sin_addr.S_un.S_un_b.s_b2,
            //    sinClient.sin_addr.S_un.S_un_b.s_b3, sinClient.sin_addr.S_un.S_un_b.s_b4);//

            serve(soClient);
        }
    } while (sin.sin_addr.S_un.S_addr == 0);
// John - you never get here.
// May be you need shutdown() same as in IGD480
    //trace("Kronos server exiting...\n");
    closesocket(so);
    return 0;
}


uint32_t WINAPI ServerThread(void *param)
{
    SioTcps *pst = (SioTcps *)param;
    return pst->Worker();
}


bool SioTcps::start()
{
  WSADATA data;
  WORD nVersion = MAKEWORD(2,1);
  DWORD id = 0;

    if (WSAStartup(nVersion, &data) != 0)
    {
        (void)WSAGetLastError();
        //trace("WSAStartup: %d [%08X]\n", dw, dw);
        return false;
    }

    thread = CreateThread(nullptr, 4096, ServerThread, (void *)this, 0, &id);
    return thread != NULL;
}


SioTcp *SioTcps::find()
{
    for (int i = 0; i < N; ++i)
    {
        if (!rgtcp[i]->connected())
            return rgtcp[i];
    }
    return NULL;
}
