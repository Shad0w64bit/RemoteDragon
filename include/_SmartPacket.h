#include <windows.h>

#pragma pack(push, 1)
struct HeaderPacket
{
    long int len;
    WORD type;
};
#pragma pack(pop)

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
            HeaderPacket* head = GetHeader();
            head->len = length;
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

    char* GetRawData()
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

    int GetDataLength()
    {
        return length - sizeof(HeaderPacket);
    }

    void PositionMove(int i)
    {
        pos += i;
    }

    void SetPosition(int i)
    {
        pos = i;
    }

    bool Complete()
    {
        return (pos == length);
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
