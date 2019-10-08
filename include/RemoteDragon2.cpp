#include "RemoteDragon2.h"

RemoteDragon2::RemoteDragon2()
{

}

RemoteDragon2::~RemoteDragon2()
{

}

void RemoteDragon2::SendMsg(const char * str)
{
    int len = strlen(str);
    SmartPacket* p = new SmartPacket();
    p->AllocateSend(len);
    memcpy( p->GetRawData(), str, p->GetLength());
    NetClient2::Send( p );
}

void RemoteDragon2::RecivePacket(SmartPacket* packet)
{
    HeaderPacket* head = packet->GetHeader();

    switch (head->type)
    {
    case 0x7367:
        if (cbReciveMsg != nullptr)
            cbReciveMsg( packet->GetData() );
        break;
    }
}

void RemoteDragon2::SendedPacket(SmartPacket* packet)
{

}
