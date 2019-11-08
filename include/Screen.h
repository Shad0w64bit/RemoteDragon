#include "Monitor.h"
#include "SmartPacket.h"

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

private:
    Monitor* device;
    MonitorItem* curScreen;
    //MonitorItem* lastScreen;


};
