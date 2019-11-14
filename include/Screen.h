#include <vector>
#include "Monitor.h"
#include "SmartPacket.h"

#define PIXEL_BLOCK 120

struct ScreenBlock
{
    int bH;
    int bW;
    int n;
    int startX;
};

class Screen
{
public:
    Screen();
    ~Screen();

    void Capture();
    void Analyze(std::vector<ScreenBlock>*& res);

    SmartPacket* GetFull();
    SmartPacket* GetChanged();
    SmartPacket* GetResolution();
    SmartPacket* CompressChanges(std::vector<ScreenBlock>* blocks);

    void SelectMonitor(int i);

protected:

    bool BlockEqual(int n);
    bool eq(int cH, int cW, int imgWidth, int startX);
    bool fill(int cH, int cW, int imgWidth, int startX, int color);

private:
    //vector<
    Monitor* device;
    MonitorItem* curScreen;
    MonitorItem* lastScreen;
    //MonitorItem* lastScreen;


};
