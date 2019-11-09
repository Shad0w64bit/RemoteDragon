#include "Monitor.h"
#include "SmartPacket.h"

#define PIXEL_BLOCK 1000

class Screen
{
public:
    Screen();
    ~Screen();

    void Capture();
    void Analyze();

    SmartPacket* GetFull();
    SmartPacket* GetChanged();
    SmartPacket* GetResolution();

    void SelectMonitor(int i);

protected:

    bool BlockEqual(int n);
    bool eq(int cH, int cW, int imgWidth, int startX);

private:
    Monitor* device;
    MonitorItem* curScreen;
    MonitorItem* lastScreen;
    //MonitorItem* lastScreen;


};
