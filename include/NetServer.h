#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdexcept>
#include <string>
#include <thread>
#include <atomic>
#include <list>
#include <queue>

#define COUT(x) std::cout<<x<<std::endl;

struct PacketHeader
{
    int length;
    unsigned int from;
    unsigned int to;
    WORD type;
};

struct Packet
{
    SOCKET socket;
    PacketHeader header;
    char* data;
    int length = 0;
    int load = 0;
};

struct DragonEgg {
    unsigned int id;
    SOCKET socket;
    std::atomic<bool> exit;
    std::thread* thread;
    Packet recive;
    std::queue<Packet*> sent;
};

class NetServer
{
public:
    NetServer(bool WSAInit = true);
    ~NetServer();

    bool Start(long unsigned int host, WORD port);
    void Stop();

private:
    bool _WSAInit;
    SOCKET _sock;
    unsigned long _host = 0x7F000001;
    WORD _port = 22222;

    unsigned int _NextID = 1;

    std::atomic<bool> _terminate;
    std::thread* _handler;

    std::list<DragonEgg*> clients;

    void Handler();
    void ClientHandler(DragonEgg& egg);

    void ClearClients(DragonEgg& egg);
};
