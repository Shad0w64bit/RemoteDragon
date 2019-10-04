#include "NetClient.h"

#define DATA_BLOCK 32768

NetClient::NetClient(bool WSAInit)
{
    _WSAInit = WSAInit;

    if (_WSAInit)
    {
        WSADATA ws;
        if ( FAILED( WSAStartup( MAKEWORD( 2, 2 ), &ws ) ) )
        {
            char* buf = reinterpret_cast<char*>(malloc(30));
            sprintf(buf, "WSAStartup error: %d", WSAGetLastError());
            throw buf;
        }
    }

//    _host = inet_addr("127.0.0.1");
    _handler = nullptr;
}

NetClient::~NetClient()
{
    Disconnect();
    if (_WSAInit)
    {
        WSACleanup();
    }
}

void NetClient::Connect(const char* host, unsigned short int port)
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

/*    if ( SOCKET_ERROR == ( bind(_sock, (sockaddr*) &addr, sizeof(addr)) ) )
    {
        char* buf = reinterpret_cast<char*>(malloc(30));
        sprintf(buf, "Socket error: %d", WSAGetLastError());
        throw buf;
    }

    if ( FAILED(listen( _sock, SOMAXCONN)) )
    {
        char* buf = reinterpret_cast<char*>(malloc(30));
        sprintf(buf, "Socket error: %d", WSAGetLastError());
        throw buf;
    }
*/
    // Run Handler
    _terminate = false;
    _handler = new std::thread(&NetClient::Handler, this);
    _handler->detach();
}

void NetClient::Disconnect()
{
    if (_handler != nullptr)
    {
        _terminate = true;
        int err = 0;
        while (_handler != nullptr)
        {
            if (err++ > 1000) // 10 second
            {
                throw std::runtime_error( "Thread exit exception. It's time too long." );
                //throw "Thread exit exception. It's time too long.";
            }
            Sleep(10);
        }
        delete _handler;
    }

    while (!_out.empty())
    {
        pkg* p= _out.front();
        //printf(p->buffer);
        free( p->buffer );
        delete p;
        _out.pop();
    }


    std::queue<pkg*> empty;
    std::swap( _out, empty );

    // Stop Handler
    closesocket(_sock);
}

void NetClient::Handler()
{
    while (!_terminate)
    {
        timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        fd_set readset, writeset;
        FD_ZERO( &readset );
        FD_ZERO( &writeset );
        FD_SET(_sock, &readset); // ?

        if (!_out.empty())
        {
            FD_SET(_sock, &writeset);
        }

        if (select(2, &readset, &writeset, NULL, &timeout) <= 0)
        {
            continue;
        }

        if (FD_ISSET(_sock, &readset))
        {
            pkg* p = nullptr;

            if (_lastInPacket == nullptr)
            {
                u_long bytes = 0;
                ioctlsocket( _sock, FIONREAD, &bytes );
                if (bytes < 4) {
                    continue;
                }

                int len = 0;
                if (SOCKET_ERROR == recv(_sock, reinterpret_cast<char*>(&len), sizeof(len), 0))
                {
                    continue;
                }

                char* buf = reinterpret_cast<char*>(malloc(len));

                p = new pkg {buf, len, 0};
                _lastInPacket = p;

                int get = recv(_sock, p->buffer, p->length, 0);
                p->sent += get;
            } else {
                p = _lastInPacket;
                int get =  recv(_sock, p->buffer + p->sent, p->length - p->sent, 0);
                p->sent += get;
            }
            int sent = p->sent;
            int length = p->length;

            if (p->sent == p->length)
            {
                /* TYPE */
                WORD t;
                memcpy(&t, p->buffer, sizeof(p->length));
                RemoveType(t);


                /* END TYPE */

                // Send to user
                RecivePacket(_lastInPacket);
                //Recive(_lastInPacket->buffer, _lastInPacket->length);
                free(_lastInPacket->buffer);
                delete _lastInPacket;
                _lastInPacket = nullptr;
                continue;
            }

        }

        if (FD_ISSET(_sock, &writeset))
        {
            pkg* p = _out.front();

            auto bytes_send = send(_sock, p->buffer+p->sent, p->length-p->sent, 0);
            p->sent += bytes_send;

            if (p->sent == p->length)
            {
                free(p->buffer);
                delete p;
                p = nullptr;
                _out.pop();
            }
            continue;

//            auto len = _lastPacket.length;

//            send(_sock, reinterpret_cast<const char*>(&len), sizeof(len), 0);
            //send(new_sock, reinterpret_cast<const char*>(&len), sizeof(len), 0);

        }



    }
    _handler = nullptr;
}

void NetClient::Send(const char* buffer, int length)
{
    int newLength = length + sizeof(unsigned int);
    char* buf = reinterpret_cast<char*>( malloc( newLength ) );
    memcpy(buf, reinterpret_cast<const char*>(&length), sizeof(length) );
    memcpy(buf+sizeof(unsigned int), buffer, length);

    _out.push( new pkg {buf, newLength, 0} );
}

void NetClient::RecivePacket(pkg* packet)
{
    /*
    6766    ClipBoard
    6780	Cursor Position
    6873	Display Image
    7083	File System
    7367	Internal Chat
    8382	System Request

    */
//    std::cout << packet->buffer << "\t" << packet->length << std::endl;

//    free(packet->buffer);
//    delete packet;
    // free(packet->buffer); // We need do it
}

void NetClient::RemoveType(WORD type)
{

}
