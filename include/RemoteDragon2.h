#include "NetClient2.h"

typedef void (*FuncReciveMsg)(const char* str);
typedef void (*FuncReciveScreen)(const char* bufer, int len);

class RemoteDragon2: public NetClient2
{
public:
    RemoteDragon2();
    ~RemoteDragon2();

    // Callback functions
    void SetReciveMsgFunc(FuncReciveMsg func) { cbReciveMsg = func; };

    void SendMsg(const char* str);

protected:
    void RecivePacket(SmartPacket* packet);
    void SendedPacket(SmartPacket* packet);

private:
    FuncReciveMsg cbReciveMsg = nullptr;

};
