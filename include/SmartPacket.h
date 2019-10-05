#include <windows.h>

struct HeaderPacket
{
    unsigned int from;
    unsigned int to;
    WORD type;
};

enum PacketType {ptSend, ptRecive};

class SmartPacket
{
public:
    SmartPacket()
    {

    }

    ~SmartPacket()
    {
        if (buffer != nullptr) {
            free(buffer);
        }
    }

    bool AllocateRecive( int szBuf )
    {
        try
        {
            type = ptRecive;
            length = szBuf;
            if (buffer != nullptr) free(buffer);
            buffer = reinterpret_cast<char*> (malloc(length));
            pos = 0;
        } catch (...) {
            return false;
        }
        return true;
    }

    bool AllocateSend( int szBuf )
    {
        try
        {
            type = ptSend;
            length = sizeof(HeaderPacket) + szBuf;
            if (buffer != nullptr) free(buffer);
            buffer = reinterpret_cast<char*> (malloc(length));
            pos = sizeof(HeaderPacket);
        } catch (...) {
            return false;
        }
        return true;
    }

    char* GetPacket()
    {
        return buffer;
    }

    HeaderPacket* GetHeader()
    {
        return reinterpret_cast<HeaderPacket*>(buffer);
    }

    char* GetData()
    {
        return (sizeof(HeaderPacket) + buffer);
    }

    char* Position()
    {
        return (buffer + pos);
    }

    int GetLength()
    {
        return length - pos;
    }

    int GetPacketLength()
    {
        return length;
    }

    void PositionMove(int i)
    {
        pos += i;
    }


    void SetPacketLength(int newLength) {
        length = newLength;
    }

private:
    char* buffer = nullptr;
    int length = 0;
    int pos;
    PacketType type;
};
