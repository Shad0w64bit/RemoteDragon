#include <list>
#include <algorithm>

#include "Monitor.h"
#include "NetClient.h"
#include "SmartPacket.h"

typedef void (*FuncReciveMsg)(const char* str);
typedef void (*FuncReciveScreen)(const char* bufer, int len);

class RemoteDragon: public NetClient
{
public:
    RemoteDragon();
    ~RemoteDragon();

    bool SendMsg(const char* str);
    void SendScreen();
    void SetReciveMsgFunc(FuncReciveMsg func);
    void SetReciveScreenFunc(FuncReciveScreen func);
    //void ReciveMsg(const char* str);
    void Refresh() { mon.RefreshImage(); }

    bool QueueType(WORD type);

protected:
    std::list<WORD> lst;

    bool SendPacket(WORD type, const char* buffer, int length);
    void RecivePacket(pkg* packet);
    void RemoveType(WORD type);
private:
    Monitor mon;

    FuncReciveMsg cbReciveMsg = nullptr;
    FuncReciveScreen cbReciveScreen = nullptr;
};
