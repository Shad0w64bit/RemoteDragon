#include "Screen.h"

Screen::Screen()
{
    curScreen = nullptr;
    lastScreen = nullptr;
    device = new Monitor();

}

Screen::~Screen()
{
    delete device;
}

void Screen::Capture()
{
    if (lastScreen != nullptr)
        free(lastScreen->buffer);
    lastScreen = curScreen;

    device->RefreshImage();
    curScreen = device->GetFullImage();
}

bool Screen::BlockEqual(int n)
{
    // Block 100x100
/*    int imgHeight = device->GetHeight();
    int imgWidth = device->GetWidth();

    int bH = height / PIXEL_BLOCK; if (height % PIXEL_BLOCK) bH++;
    int bW = width / PIXEL_BLOCK; if (width % PIXEL_BLOCK) bW++;

    if ( 0 > n || n > bH*bW )
        return true;

    //        Last Element
    int cH = (n % bH == 0) ? (imgHeight % PIXEL_BLOCK) : PIXEL_BLOCK;
    int cW = (n % bW == 0) ? (imgWidth % PIXEL_BLOCK) : PIXEL_BLOCK;
*/



    return false;
}

bool Screen::eq(int cH, int cW, int imgWidth, int startX)
{

    for (int y=0; y<cH; y=y+4) // y++
    {
        int offset = (y * imgWidth*3 + startX*3);
        if (memcmp(curScreen->buffer + offset, lastScreen->buffer + offset, cW*3) != 0)
            return false;
    }
    return true;
}

bool Screen::fill(int cH, int cW, int imgWidth, int startX, int color)
{

    for (int y=0; y<cH; y++)
    {
        int offset = (y * imgWidth*3 + startX*3);
        memset(curScreen->buffer + offset, color, cW*3);
    }
}

void Screen::Analyze()
{
    if (lastScreen == nullptr)
        return;


    int imgHeight = device->GetHeight();
    int imgWidth = device->GetWidth();

    int bH = imgHeight / PIXEL_BLOCK; if (imgHeight % PIXEL_BLOCK) bH++;
    int bW = imgWidth / PIXEL_BLOCK; if (imgWidth % PIXEL_BLOCK) bW++;



    int color = 0;
    for (int n=1; n<=bH*bW; n++)
    {
        //        If Last Element
        int cH = (n > ((bH-1) * bW) && imgHeight % PIXEL_BLOCK != 0) ? (imgHeight % PIXEL_BLOCK) : PIXEL_BLOCK;
        int cW = (n % bW == 0 && (imgWidth  % PIXEL_BLOCK != 0)) ? (imgWidth  % PIXEL_BLOCK) : PIXEL_BLOCK;

        int startX;
        if (n <= bW) {
            startX = PIXEL_BLOCK * (n-1);
        } else {
            startX = imgWidth * PIXEL_BLOCK * ((n-1) / bW) + PIXEL_BLOCK * ((n-1) % bW);
        }

        /*for (int y=0; y<cH; y++)
        {
            int offset = y * imgWidth + startX;
            memcmp(curScreen->buffer + offset, lastScreen->buffer + offset, cW);
        }*/
        bool e = eq(cH, cW, imgWidth, startX);
        //fill(cH, cW, imgWidth, startX, color);
        color += 20;


        //std::cout << n  << "\t" << e << std::endl;
    }

}

void Screen::SelectMonitor(int i)
{
    device->SetMonitor(i);
}

SmartPacket* Screen::GetFull()
{
    Capture();
    Analyze();

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

    int lol;
    SmartPacket* sp = new SmartPacket();
    sp->AllocateSend( sizeof(lol) );
    char* buffer = sp->GetData();
    memcpy(buffer, &lol, sizeof(lol));
    sp->GetHeader()->type = 0x6879;
    return sp;
}
