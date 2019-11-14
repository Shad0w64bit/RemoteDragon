#include "RemoteDragon2.h"

RemoteDragon2::RemoteDragon2()
{
    _heightScreen = 0;
    _widthScreen = 0;
    screenSending = true;
    screen.SelectMonitor(2);
//    int cnt = mon.Count();
//    mon.SetMonitor(2);
}

RemoteDragon2::~RemoteDragon2()
{

}



void RemoteDragon2::SendMsg(const char * str)
{
    int len = strlen(str) + 1;
    SmartPacket* p = new SmartPacket();
    p->AllocateSend(len);
    memcpy( p->GetData(), str, len);

    HeaderPacket* h= p->GetHeader();
    h->type = 0x7367;

    NetClient2::Send( p );
}

void RemoteDragon2::RecivePacket(SmartPacket* packet)
{
    switch (packet->GetHeader()->type)
    {
    case 0x7367:
        if (cbReciveMsg != nullptr)
            cbReciveMsg( packet->GetData() );
        break;
    case 0x6873:
        if (cbReciveScreen != nullptr)
            cbReciveScreen(packet->GetData(), packet->GetDataLength());
        break;
    case 0x6874: // Resize Window
        {
            char* buf = packet->GetData();
            memcpy(&_heightScreen, buf, sizeof(_heightScreen));
            memcpy(&_widthScreen, buf + sizeof(_heightScreen), sizeof(_widthScreen));

        }
        break;
    }
}

void RemoteDragon2::SendedPacket(SmartPacket* packet)
{
    if ((packet->GetHeader()->type == 0x6873) ||
        (packet->GetHeader()->type == 0x6874) ||
        (packet->GetHeader()->type == 0x6879))
    {
        screenSending = false;
    }
}

void RemoteDragon2::SendScreen()
{
    if (screenSending) {
        return;
    }

    screenSending = true;
/*
    mon.Refresh();
    MonitorItem* mi = mon.GetFullImage();

    SmartPacket* sp = new SmartPacket();
    sp->AllocateSend(mi->len);
//    char* buf = mi->buffer;
    memcpy(sp->GetData(), mi->buffer, mi->len);
    sp->GetHeader()->type = 0x6873;
    NetClient2::Send(sp);
//    SendPacket(0x6873, reinterpret_cast<char*>(mi->buffer), mi->len);
    free(mi->buffer);
    delete mi;
*/
    static bool start = true;
    SmartPacket* sp;
    if (start) {
        sp = screen.GetFull();
        start = !start;
    } else {
        sp = screen.GetChanged();
    }

//    SmartPacket* sp = screen.GetChanged();
//    SmartPacket* sp = screen.GetFull();
    if (sp != nullptr) {
        NetClient2::Send(sp);
    } else {
        screenSending = false;
    }

    Sleep(100);
}

void RemoteDragon2::SendResolution()
{
    SmartPacket* sp = screen.GetResolution();
    NetClient2::Send(sp);
/*    screenSending = true;
    mon.DeviceRefresh();

    int h = mon.GetHeight();
    int w = mon.GetWidth();

    SmartPacket* sp = new SmartPacket();
    sp->AllocateSend( sizeof(h) + sizeof(w) );
    char* buffer = sp->GetData();
    memcpy(buffer, &h, sizeof(h));
    memcpy(buffer+sizeof(h), &w, sizeof(w));
    sp->GetHeader()->type = 0x6874;
    NetClient2::Send(sp);*/

}

int RemoteDragon2::GetHeight()
{
    return _heightScreen;
}

int RemoteDragon2::GetWidth()
{
    return _widthScreen;
}
