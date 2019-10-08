#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <queue>
#include "SmartPacket.h"


class NetClient2
{
public:
    NetClient2(bool WSAInit = true);
    ~NetClient2();

    void Connect(const char* host, WORD port);
    void Disconnect();

    void Send(SmartPacket* packet);

protected:
    SOCKET _sock;
    SmartPacket* _in;
    std::queue<SmartPacket*> _out;

    virtual void RecivePacket(SmartPacket* packet) {};
    virtual void SendedPacket(SmartPacket* packet) {};

private:
    bool _WSAInit;
    bool _terminate;

    std::thread* _thRead;
    std::thread* _thWrite;

    void ReadThread();
    void WriteThread();

};
