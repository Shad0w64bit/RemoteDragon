#include "NetClient2.h"

NetClient2::NetClient2(bool WSAInit)
{
    _WSAInit = WSAInit;

    if (_WSAInit)
    {
        WSADATA ws;
        if ( FAILED( WSAStartup( MAKEWORD( 2, 2 ), &ws ) ) )
        {
            throw std::runtime_error( "WSAStartup error: " + std::to_string( WSAGetLastError() ) );
        }
    }

//    _host = inet_addr("127.0.0.1");
//    _handler = nullptr;
}

NetClient2::~NetClient2()
{
    Disconnect();
    if (_WSAInit)
    {
        WSACleanup();
    }
}

void NetClient2::Connect(const char* host, WORD port)
{
    Disconnect();

    if ( INVALID_SOCKET == (_sock = socket(AF_INET, SOCK_STREAM, 0)) )
    {
        throw std::runtime_error( "Socket create error: " + std::to_string( WSAGetLastError() ) );
    }

    struct sockaddr_in addr;
    ZeroMemory( &addr, sizeof(addr) );
    addr.sin_family = AF_INET; // TCP/IP
    addr.sin_addr.S_un.S_addr = inet_addr(host); //("127.0.0.1")
    addr.sin_port = htons(port);

    if ( SOCKET_ERROR == ( connect(_sock, (sockaddr*) &addr, sizeof(addr)) ) )
    {
        throw std::runtime_error( "Socket connection error: " + std::to_string( WSAGetLastError() ) );
    }

    u_long iMode = 1; // NON-BLOCKING
    ioctlsocket( _sock, FIONBIO, &iMode );

    _terminate = false;

    _thRead = new std::thread(&NetClient2::ReadThread, this);
    _thWrite = new std::thread(&NetClient2::WriteThread, this);
    _thRead->detach();
    _thWrite->detach();
}

void NetClient2::Disconnect()
{
    _terminate = true;

    int err = 0;
    while (_thRead != nullptr && _thWrite != nullptr)
    {
        if (err++ > 1000) // 10 second
        {
            throw std::runtime_error( "Thread exit exception. It's time too long." );
        }
        Sleep(10);
    }
    delete _thRead;  _thRead  = nullptr;
    delete _thWrite; _thWrite = nullptr;

    // Clear input packet

    if (_in != nullptr)
    {
        free(_in);
        _in = nullptr;
    }

    // Clear output queue

    while (!_out.empty())
    {
        SmartPacket* sp = _out.front();
        free(sp);
        _out.pop();
    }

    closesocket(_sock);
}

void NetClient2::ReadThread()
{
    while (!_terminate)
    {
        timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        fd_set readset;
        FD_ZERO( &readset );
        FD_SET( _sock, &readset );

        if (select(2, &readset, NULL, NULL, &timeout) <= 0)
        {
            continue;
        }

        if (FD_ISSET(_sock, &readset))
        {
            if (_in == nullptr) {

                u_long bytes = 0;
                ioctlsocket( _sock, FIONREAD, &bytes );
                if (bytes < sizeof(HeaderPacket)) {
                    continue;
                }

                HeaderPacket head;
                if (SOCKET_ERROR == recv(_sock, reinterpret_cast<char*>(&head), sizeof(HeaderPacket), 0))
                {
                    continue;
                }

                _in = new SmartPacket;
                _in->AllocateRecive(head.len);
                memcpy(_in->GetHeader(), &head, sizeof(HeaderPacket));
                _in->SetPosition(sizeof(HeaderPacket));
            }

            auto bytes_recv = recv(_sock, _in->GetRawData(), _in->GetLength(), 0);
            _in->PositionMove(bytes_recv);

            if (_in->Complete())
            {
//                auto d = _in->GetData();
                RecivePacket(_in);
                delete _in;
                _in = nullptr;
            }
            continue;

        }

    }
    _thRead = nullptr;
}

void NetClient2::WriteThread()
{
    while (!_terminate)
    {
        if (_out.empty())
        {
            Sleep(200);
            continue;
        }

        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 200000;

        fd_set writeset;
        FD_ZERO( &writeset );
        FD_SET( _sock, &writeset );

        if (select(2, NULL, &writeset, NULL, &timeout) <= 0)
        {
            continue;
        }

        if (FD_ISSET(_sock, &writeset))
        {
            SmartPacket* p = _out.front();

//            auto d = p->GetRawData();
            auto bytes_send = send(_sock, p->GetRawData(), p->GetLength(), 0);
            p->PositionMove(bytes_send);

            if (p->Complete())
            {
                SendedPacket(p);
                delete p;
                _out.pop();
            }
            continue;
        }


    }
    _thWrite = nullptr;
}

void NetClient2::Send( SmartPacket* packet )
{
    packet->SetPosition(0);
    _out.push(packet);
}
