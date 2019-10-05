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
    SmartPacket() //int bufLen = 0
    {
/*        length = bufLen;
        if (length > 0) {
            buffer = reinterpret_cast<char*>( malloc(length) );
        }*/

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
            length = szBuf;
            if (buffer != nullptr) free(buffer);
            int szHeader = sizeof(HeaderPacket);
            buffer = reinterpret_cast<char*> (malloc(szHeader + length));
            pos = szHeader;
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


/*
    int GetPosition()
    {
        return pos;
    }

    int GetLength()
    {
        return length - pos;
    }

    void SetPosition(int p)
    {
        pos = p;
    }

    void AddPosition(int p)
    {
        pos += p;
    }

    */
/*
    HeaderPacket* GetHeaderPacker()
    {
        return reinterpret_cast<HeaderPacket*>(buffer);
    }
*/



/*    void Resize( int len )
    {
        // SLOWLY
        int oldLen = length;
        int newLen = ( len > oldLen ) ? oldLen : len ;
        char* b = buffer;
        char* n = reinterpret_cast<char*> (malloc(len));

        if (b == nullptr)
        {
            buffer = n;
            return;
        }

        memcpy( n, b, newLen );

        free(buffer);
        buffer = n;
    }*/



    void SetLength() { // int newLength = 0
        // length = newLength;
    }

private:
    char* buffer = nullptr;
    int length = 0;
    int pos;
    PacketType type;
//    HeaderPacket* header = nullptr;



};
