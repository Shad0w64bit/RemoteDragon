#include "NetServer.h"

NetServer::NetServer(bool WSAInit)
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

    _handler = nullptr;
}

NetServer::~NetServer()
{
    Stop();
    if (_WSAInit)
    {
        WSACleanup();
    }
}

bool NetServer::Start(long unsigned int host, WORD port)
{
    try
    {
        Stop();

        _host = host;
        _port = port;

        if ( INVALID_SOCKET == (_sock = socket(AF_INET, SOCK_STREAM, 0)) )
        {
            throw std::runtime_error( "Socket create error: " + std::to_string( WSAGetLastError() ) );
        }

        struct sockaddr_in addr;
        ZeroMemory( &addr, sizeof(addr) );
        addr.sin_family = AF_INET; // TCP/IP
        addr.sin_addr.S_un.S_addr = _host;//inet_addr(_host)
        addr.sin_port = htons(_port);

        if ( SOCKET_ERROR == ( bind(_sock, (sockaddr*) &addr, sizeof(addr)) ) )
        {
            throw std::runtime_error( "Socket bind error: " + std::to_string( WSAGetLastError() ) );
        }

        if ( FAILED(listen( _sock, SOMAXCONN)) )
        {
            throw std::runtime_error( "Socket listen error: " + std::to_string( WSAGetLastError() ) );
        }

        u_long iMode = 1; // NON-BLOCKING
        ioctlsocket( _sock, FIONBIO, &iMode );

        _terminate = false;
        _handler = new std::thread(&NetServer::Handler, this);
        _handler->detach();

        return true;
    } catch (const std::runtime_error& e) {
        throw e;
    }
    return false;
}

void NetServer::Stop()
{

}

void NetServer::Handler()
{
    while (!_terminate)
    {
        timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        fd_set readset;
        FD_ZERO( &readset );
        FD_SET(_sock, &readset);

        if (select(2, &readset, NULL, NULL, &timeout) <= 0)
        {
            continue;
        }


        if (FD_ISSET( _sock, &readset ))
        {
            /* Connection request on original socket. */
            SOCKET new_sock = accept(_sock, NULL, NULL);
            u_long iMode = 1; // NON-BLOCKING
            ioctlsocket( new_sock, FIONBIO, &iMode );

            DragonEgg* egg = new DragonEgg();
            egg->id = _NextID++;
            egg->socket = new_sock;
            egg->exit = false;
            egg->thread = new std::thread( &NetServer::ClientHandler, this, std::ref(*egg) );
            egg->thread->detach();

            clients.insert( clients.end(), egg );
//            socks.insert( std::make_pair(new_sock, "") );
//            cout << "Add " << new_sock << endl;

            COUT( "Connect \t" << new_sock );

            Sleep(200);
            COUT( "\n\tList" );
            for (auto it=clients.begin(); it != clients.end(); it++) {
                COUT( (*it)->id << "\tsocket\t" << (*it)->socket  );
            }
            continue;
        }



    }

    _handler = nullptr;
}

void NetServer::ClientHandler(DragonEgg& egg)
{
    while (!egg.exit)
    {
        timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        fd_set readset, writeset;
        FD_ZERO( &readset );
        FD_SET(egg.socket, &readset);

        if (!egg.sent.empty())
        {
            FD_ZERO(&writeset);
            FD_SET(egg.socket, &writeset);
        }

        if (select(2, &readset, &writeset, NULL, &timeout) <= 0)
        {
            continue;
        }

        if (FD_ISSET(egg.socket, &readset))
        {
            if (egg.recive.length < sizeof(PacketHeader)) {
                int bytes_read = recv(egg.socket, reinterpret_cast<char*>(&egg.recive.header + egg.recive.load), sizeof(PacketHeader) - egg.recive.load, 0);
                egg.recive.load += bytes_read;
                if (egg.recive.load < sizeof(PacketHeader)) {
                    continue;
                } else { // Received Header
                    egg.recive.data = reinterpret_cast<char*>(malloc(egg.recive.header.length));
                }
            }

            int bytes_read = recv(egg.socket, egg.recive.data-sizeof(PacketHeader), 65535, 0);
            if (bytes_read <= 0) {
                 // Client disconnect
                 break;
            }



        }




    }

    COUT( "Disconnect \t" << egg.socket );
    closesocket( egg.socket );
    egg.thread = nullptr;
    this->ClearClients(egg);
}
void NetServer::ClearClients(DragonEgg& egg)
{
    COUT( "Search\t" << egg.socket );
//    clients.remove(egg);
    for (auto it=clients.begin(); it != clients.end(); it++) {
        if ((*it)->id == egg.id && egg.thread == nullptr) {
            clients.erase(it);
            return;
        }
    }
}
