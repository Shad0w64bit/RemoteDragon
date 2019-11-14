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
    return true;
}

void Screen::Analyze(std::vector<ScreenBlock>*& res)
{
    res = nullptr;
    if (lastScreen == nullptr)
        return;


    int imgHeight = device->GetHeight();
    int imgWidth = device->GetWidth();

    int bH = imgHeight / PIXEL_BLOCK; if (imgHeight % PIXEL_BLOCK) bH++;
    int bW = imgWidth / PIXEL_BLOCK; if (imgWidth % PIXEL_BLOCK) bW++;



    std::vector<ScreenBlock>* r = new std::vector<ScreenBlock>();
//    int color = 0;
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
        if (!e) {
            r->insert(r->end(), {cH, cW, n, startX});
        }
        //fill(cH, cW, imgWidth, startX, color);
        //color += 20;


        //std::cout << n  << "\t" << e << std::endl;
    }

    if (r->empty())
    {
        delete r;
        r = nullptr;
    }
    res = r;
}

void Screen::SelectMonitor(int i)
{
    device->SetMonitor(i);
}

SmartPacket* Screen::GetFull()
{
    Capture();
//    Analyze();

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

SmartPacket* Screen::CompressChanges(std::vector<ScreenBlock>* blocks)
{
    int lenPacket = 0;
    for (auto it = blocks->begin(); it != blocks->end(); it++)
    {
        lenPacket += sizeof(WORD) + sizeof(int) + (*it).bH * (*it).bW * 3;
    }

//    int len = cntBlock * (PIXEL_BLOCK * PIXEL_BLOCK + sizeof(WORD) + sizeof(int));
    SmartPacket* sp = new SmartPacket();
    sp->AllocateSend(lenPacket);
    char* buffer = sp->GetData();

    int imgWidth = device->GetWidth();
    int offset = 0;

    for (auto it = blocks->begin(); it != blocks->end(); it++)
    {
        int len = (*it).bH * (*it).bW;
        WORD n = (*it).n;
        memcpy(buffer+offset, &n, sizeof(WORD));
        memcpy(buffer+offset+sizeof(WORD), &len, sizeof(int));
        offset += sizeof(WORD) + sizeof(int);

        for (int h=0; h<(*it).bH; h++)
        {
            int imgOffset = (*it).startX + h*imgWidth*3;
            memcpy(buffer+offset, curScreen->buffer + imgOffset, (*it).bW * 3);
            offset += (*it).bW*3;
        }
    }

    sp->GetHeader()->type = 0x6879;
    return sp;
/*    for (int offset = 0; offset < lenPacket; offset += PIXEL_BLOCK * PIXEL_BLOCK + sizeof(WORD) + sizeof(int)) {
        //memcpy()

    }*/

}

SmartPacket* Screen::GetChanged()
{
    Capture();
    std::vector<ScreenBlock>* res = nullptr;
    Analyze(res);

    if (res == nullptr) {
        return nullptr;
    }

    SmartPacket* sp = CompressChanges(res);
    res->clear();
    delete res;

    return sp;
/*    int lol;
    SmartPacket* sp = new SmartPacket();
    sp->AllocateSend( sizeof(lol) );
    char* buffer = sp->GetData();
    memcpy(buffer, &lol, sizeof(lol));
    sp->GetHeader()->type = 0x6879;
    return sp;*/
}
