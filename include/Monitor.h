#define _DEBUG
#ifndef UNICODE
#define UNICODE
#endif // UNICODE

#include <windows.h>
#include <map>
#include <atomic>

#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

struct MonitorIndex
{
    int Index;
    int Count;
    MONITORINFO MonitorInfo;
};

struct MonitorItem
{
    char* buffer;
    int len;
};

class Monitor
{
public:
    Monitor();
    ~Monitor();

    bool DeviceRefresh();
    int Count();

    void Refresh();

    void SetMonitor(int monitor);
    MonitorItem* GetFullImage();

    int GetWidth();
    int GetHeight();

protected:

private:
    int activeMonitor;
    HBITMAP curScreen;
    HBITMAP lastScreen;

    int blockHeightCount;
    int blockWidthCount;

    std::map<int, MonitorIndex> _monitors;

    void CalcBlock();
    static BOOL __stdcall MonitorAllCallback(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
    void CaptureDeviceContext(HDC srcDC, LPRECT rect, HBITMAP destBitmap);

};

