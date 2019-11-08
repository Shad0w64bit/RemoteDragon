#include "RemoteDragon.h"

RemoteDragon::RemoteDragon()
{


}

RemoteDragon::~RemoteDragon()
{

}

void RemoteDragon::RecivePacket(pkg* packet)
{
    std::cout << packet->length << std::endl;
    WORD type;
    memcpy(&type, packet->buffer, sizeof(type));

    char* link = packet->buffer + sizeof(type);
    int len = packet->length - sizeof(type);

    switch (type)
    {
    case 0x7367:
        if (cbReciveMsg != nullptr)
        {
            cbReciveMsg(link);
        }
        break;
    case 0x6873:
        if (cbReciveScreen != nullptr)
        {
            cbReciveScreen(link, len);
        }

    }
}

bool RemoteDragon::SendPacket(WORD type, const char* buffer, int length)
{
    int newLength = length + sizeof(type);
    char* newBuffer = reinterpret_cast<char*>( malloc(newLength) );
    memcpy(newBuffer, &type, sizeof(type));
    memcpy(newBuffer + sizeof(type), buffer, length);

    lst.insert(lst.end(), type);

    NetClient::Send( newBuffer, newLength);
    free(newBuffer);
    return true;
}

bool RemoteDragon::SendMsg(const char* str)
{
    int len = strlen(str)+1;
    SendPacket(0x7367, str, len);
    return true;
}

void RemoteDragon::SendScreen()
{
    if (QueueType(0x6873))
    {
        return;
    }
/*    if (_out.size() > 1) {
        return;
    }*/
//    BYTE* buf;
//    int len;
    mon.RefreshImage();
    MonitorItem* mi = mon.GetFullImage();
//    mon.GetFullImage(*buf, &len);
//    std::cout << mon.Count() << std::endl;
    SendPacket(0x6873, reinterpret_cast<char*>(mi->buffer), mi->len);
    free(mi->buffer);
    delete mi;
//    free(buf);
}

void RemoteDragon::SetReciveMsgFunc(FuncReciveMsg func)
{
    cbReciveMsg = func;
}

void RemoteDragon::SetReciveScreenFunc(FuncReciveScreen func)
{
    cbReciveScreen = func;
}

void RemoteDragon::RemoveType(WORD type)
{
    auto it = std::find(lst.begin(), lst.end(), type);
    if (it != lst.end())
    {
        lst.erase(it);
    }
}

bool RemoteDragon::QueueType(WORD type)
{
    auto it = std::find(lst.begin(), lst.end(), type);
    return (it != lst.end());
}
