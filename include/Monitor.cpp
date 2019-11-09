#include "Monitor.h"

Monitor::Monitor()
{
    activeMonitor = -1;
    DeviceRefresh();
}

Monitor::~Monitor()
{
    DeleteObject(lastScreen);
    DeleteObject(curScreen);
    _monitors.clear();

}

BOOL Monitor::MonitorAllCallback(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    std::map<int, MonitorIndex>* lst = reinterpret_cast< std::map<int, MonitorIndex>* >(dwData);

    MonitorIndex mi;
    mi.Index = lst->size() + 1;
    mi.MonitorInfo.cbSize = sizeof(mi.MonitorInfo);
    GetMonitorInfo(hMonitor,  &mi.MonitorInfo);
    (*lst)[mi.Index] = mi;
    return true;
}

bool Monitor::DeviceRefresh()
{
    try
        {
            EnumDisplayMonitors( NULL, NULL, &MonitorAllCallback, reinterpret_cast<LPARAM>(&_monitors));
            if (activeMonitor == -1 && Count() > 0)
            {
                auto it = _monitors.begin();
                SetMonitor(it->first);
            }
            #ifdef _DEBUG
            for (auto it = _monitors.begin(); it != _monitors.end(); ++it)
            {
                std::cout << "Monitor " << it->first << "\t" <<
                    it->second.MonitorInfo.rcMonitor.right - it->second.MonitorInfo.rcMonitor.left << "x" <<
                    it->second.MonitorInfo.rcMonitor.bottom - it->second.MonitorInfo.rcMonitor.top << std::endl;
            }
            std::cout << "Monitor " << activeMonitor << " is active" << std::endl;
            #endif // _DEBUG
        }
        catch(...)
        {
            return false;
        }

        return true;
}

int Monitor::Count()
{
    return _monitors.size();
}

void Monitor::SetMonitor(int monitor)
{
    if (_monitors.find(monitor) != _monitors.end()) // Found
    {
        activeMonitor = monitor;
    }
    CalcBlock();
}

void Monitor::CaptureDeviceContext(HDC srcDC, LPRECT rect, HBITMAP destBitmap)
{
    int width = rect->right - rect->left;
    int height = rect->bottom - rect->top;

    HDC hdcMem = CreateCompatibleDC(srcDC);
    HGDIOBJ prevObj = SelectObject( hdcMem, destBitmap );

    BitBlt( hdcMem, 0, 0, width, height, srcDC, rect->left, rect->top, SRCCOPY );

    SelectObject( hdcMem, prevObj );
    DeleteDC(hdcMem);
}

void Monitor::RefreshImage()
{
    DeleteObject(lastScreen);
    lastScreen = curScreen;
    HDC dc = CreateDC(L"DISPLAY", NULL, NULL, NULL);
    LPRECT r = &_monitors.at(activeMonitor).MonitorInfo.rcMonitor;
    int height = r->bottom - r->top;
    int width = r->right - r->left;
    curScreen = CreateCompatibleBitmap(dc,
                                        width,
                                        height);
    CaptureDeviceContext( dc, r, curScreen);
    ReleaseDC(0,dc);
}


MonitorItem* Monitor::GetFullImage()
{
/*    LPRECT r = &_monitors.at(activeMonitor).MonitorInfo.rcMonitor;
    int width = r->right - r->left;
    int height = r->bottom - r->top;*/

    HDC hdc = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdc);

    BITMAPINFO MyBMInfo = {0};
    MyBMInfo.bmiHeader.biSize = sizeof(MyBMInfo.bmiHeader);
    // Get the BITMAPINFO structure from the bitmap
    if(0 == GetDIBits(hdcMem, curScreen, 0, 0, NULL, &MyBMInfo, DIB_RGB_COLORS)) {
        throw "Error create bitmap";
        //std::cout << "error create image" << std::endl;
    }

    BYTE* lpPixels = new BYTE[MyBMInfo.bmiHeader.biSizeImage];
    MyBMInfo.bmiHeader.biSize = sizeof(MyBMInfo.bmiHeader);
    MyBMInfo.bmiHeader.biBitCount = 24; // 24 fix
    MyBMInfo.bmiHeader.biCompression = BI_RGB;
    MyBMInfo.bmiHeader.biHeight = abs(MyBMInfo.bmiHeader.biHeight);

    // get the actual bitmap buffer
    if(0 == GetDIBits(hdc, curScreen, 0, MyBMInfo.bmiHeader.biHeight, (LPVOID)lpPixels, &MyBMInfo, DIB_RGB_COLORS)) {
        throw "Error getting bitmap information";
    }

    int szBuffer = MyBMInfo.bmiHeader.biHeight * MyBMInfo.bmiHeader.biWidth * MyBMInfo.bmiHeader.biBitCount / 8;
    BYTE* buffer = reinterpret_cast<BYTE*>(malloc(szBuffer));
    memcpy(buffer, lpPixels, szBuffer);

/*    szBuffer = 3*width*height;
    buffer = reinterpret_cast<BYTE*>(malloc(szBuffer));//new BYTE[szBuffer];

    int _write = 0;
    for (int y = 0; y < height*width*4; y+=width*4)
    {
        for (int x = 0; x < width*4; x=x+4)
        {
            memcpy(&buffer[_write], &lpPixels[y+x+1], 3);
            _write += 3;
        }
    }*/
    delete[] lpPixels;


/*
    char pixel[4];
    pixel[0] = 0;
    pixel[1] = 0; // Red
    pixel[2] = 0; // Green
    pixel[3] = 0; // Blue
*/

    DeleteDC(hdcMem);
    DeleteDC(hdc);
    ReleaseDC(0,hdcMem);
    ReleaseDC(0,hdc);
    MonitorItem* mi = new MonitorItem{reinterpret_cast<char*>(buffer), szBuffer};
    return mi;
}



void Monitor::CalcBlock()
{
    LPRECT r = &_monitors.at(activeMonitor).MonitorInfo.rcMonitor;
    int width = r->right - r->left;
    int height = r->bottom - r->top;

    blockWidthCount = width / 100;
    if (width % 100 > 0)
        blockWidthCount++;

    blockHeightCount = height / 100;
    if (height % 100 > 0)
        blockHeightCount++;
}

int Monitor::GetWidth()
{
    LPRECT r = &_monitors.at(activeMonitor).MonitorInfo.rcMonitor;
    return (r->right - r->left);
}

int Monitor::GetHeight()
{
    LPRECT r = &_monitors.at(activeMonitor).MonitorInfo.rcMonitor;
    return (r->bottom - r->top);
}
