#include "NetClient2.h"
#include "Monitor.h"
#include <atomic>

typedef void (*FuncReciveMsg)(const char* str);
typedef void (*FuncReciveScreen)(const char* bufer, int len);

class RemoteDragon2: public NetClient2
{
public:
    RemoteDragon2();
    ~RemoteDragon2();

    // Callback functions
    void SetReciveMsgFunc(FuncReciveMsg func) { cbReciveMsg = func; }
    void SetReciveScreenFunc(FuncReciveScreen func) { cbReciveScreen = func; }

    void SendMsg(const char* str);
    void SendScreen();

    void SendResolution();
    int GetHeight();
    int GetWidth();

protected:
    void RecivePacket(SmartPacket* packet);
    void SendedPacket(SmartPacket* packet);

private:
    Monitor mon;
    int _heightScreen;
    int _widthScreen;
    std::atomic<bool> screenSending;

    FuncReciveMsg cbReciveMsg = nullptr;
    FuncReciveScreen cbReciveScreen = nullptr;

};
