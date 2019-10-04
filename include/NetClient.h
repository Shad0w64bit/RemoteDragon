#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <iostream> // cout

typedef std::function <void(char*, unsigned int)> ReceiveCallback;

struct pkg
{
    char* buffer;
    int length;
    int sent;
};

class NetClient
{
public:
    NetClient(bool WSAInit = true);
    ~NetClient();

    void Connect(const char* host, unsigned short int port);
    void Disconnect();

    void Send(const char* buffer, int length);
    //void Receive(char* buf, unsigned int len)
protected:
    std::queue<pkg*> _out;

    virtual void RecivePacket(pkg* packet);
    virtual void RemoveType(WORD type);
private:
    unsigned long _host = 0x7F000001;
    short int _port = 22222;
    SOCKET _sock;
    bool _WSAInit;

    std::atomic<bool> _terminate;
    std::thread* _handler;

//    int _lastSend = 0;
//    int _lastReceive = 0;
//    pkg* _lastOutPacket = nullptr;
    pkg* _lastInPacket = nullptr;

    void Handler();
};
