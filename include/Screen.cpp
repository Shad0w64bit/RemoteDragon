#include "Screen.h"

Screen::Screen()
{
    curScreen = nullptr;
    device = new Monitor();

}

Screen::~Screen()
{
    delete device;
}

void Screen::Capture()
{
    if (curScreen != nullptr)
        free(curScreen->buffer);
    device->RefreshImage();
    curScreen = device->GetFullImage();
}

void Screen::Analyze()
{

}

void Screen::SelectMonitor(int i)
{
    device->SetMonitor(i);
}

SmartPacket* Screen::GetFull()
{
    Capture();

    SmartPacket* sp = new SmartPacket();
    sp->AllocateSend(curScreen->len);
//    char* buf = mi->buffer;
    memcpy(sp->GetData(), curScreen->buffer, curScreen->len);
    sp->GetHeader()->type = 0x6873;
//    NetClient2::Send(sp);
//    SendPacket(0x6873, reinterpret_cast<char*>(mi->buffer), mi->len);
//    free(curScreen->buffer);
//    delete mi;
    return sp;
}

SmartPacket* Screen::GetResolution()
{
    device->DeviceRefresh();

    int h = device->GetHeight();
    int w = device->GetWidth();

    SmartPacket* sp = new SmartPacket();
    sp->AllocateSend( sizeof(h) + sizeof(w) );
    char* buffer = sp->GetData();
    memcpy(buffer, &h, sizeof(h));
    memcpy(buffer+sizeof(h), &w, sizeof(w));
    sp->GetHeader()->type = 0x6874;
//    NetClient2::Send(sp);
    return sp;
}

SmartPacket* Screen::GetChanged()
{
    Capture();
    Analyze();


}
