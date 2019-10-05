#include <iostream>
#include "../../include/SmartPacket.h"

#define TEST_PACKET_SIZE 32768

using namespace std;

bool checkPacket(char* orig, char* buf, int len)
{
    int* o = reinterpret_cast<int*>(orig);
    int* b = reinterpret_cast<int*>(buf);
    for (int i=0; i<len; i=i+sizeof(int))
    {
        if (*o != *b) {
            return false;
        }
        o++;
        b++;
    }
    return true;

}

void fillBuffer(char* orig, int len);
bool firstCheck(char* orig, int len);
bool secondCheck(char* orig, int len);
bool thirdCheck(char* orig, int len);

int main()
{
    char* testPacket = reinterpret_cast<char*>(malloc(TEST_PACKET_SIZE));
    fillBuffer(testPacket, TEST_PACKET_SIZE);

    if (checkPacket(testPacket, testPacket, TEST_PACKET_SIZE))
    {
        cout << "Test check function: OK"  << endl;
    } else {
        cout << "Test check function: Fail"  << endl << "Abort";
        return -1;
    }

    firstCheck(testPacket, TEST_PACKET_SIZE);

    secondCheck(testPacket, TEST_PACKET_SIZE);

    thirdCheck(testPacket, TEST_PACKET_SIZE);

    /*
    cout << "Create Test SmartPacket" << endl;

    SmartPacket sp;

    cout << "Allocate Receive SmartPacket" << endl;

    sp.AllocateRecive(TEST_PACKET_SIZE);
**/

    free(testPacket);
    return 0;
}

void fillBuffer(char* orig, int len)
{
    int* p = reinterpret_cast<int*>(orig);

    int cnt = 0;
    for (int i=0; i<TEST_PACKET_SIZE; i=i+sizeof(int))
    {
        *p = cnt++;
        p++;
    }
}

bool firstCheck(char* orig, int len)
{
    char* newPacket = reinterpret_cast<char*>(malloc(TEST_PACKET_SIZE));
    fillBuffer(newPacket, TEST_PACKET_SIZE);

    if (checkPacket(orig, newPacket, len))
    {
        cout << "Test11: " << "OK" << endl;
    } else {
        cout << "Test11: " << "Fail" << "(Packets must be equal)" << endl;
        return false;
    }

    int* i = reinterpret_cast<int*>(newPacket);
    *i = -1;

    if (!checkPacket(orig, newPacket, len))
    {
        cout << "Test12: " << "OK" << endl;
    } else {
        cout << "Test12: " << "Fail" << "(Packets must be not equal)" << endl;
        return false;
    }

    free(newPacket);
    return true;
}

bool secondCheck(char* orig, int len)
{
    SmartPacket sp;
    sp.AllocateRecive(sizeof(HeaderPacket) + TEST_PACKET_SIZE);
    HeaderPacket* head = sp.GetHeader();

    head->from = 4040;
    head->to = 3131;
    head->type = 47351;

    sp.PositionMove( sizeof(HeaderPacket) );

    int step = len / 4;

//    memcpy(sp.GetHeader(), &head, sizeof(HeaderPacket));

    for (int i=0; i< len; i+=step)
    {
        char* tmp = orig + i;
        memcpy( sp.Position(), tmp, step); // sp.GetLength()
        sp.PositionMove(step);
//        sp.AddPosition(step);
    }

    if (checkPacket(orig, sp.GetData(), TEST_PACKET_SIZE))
    {
        cout << "Test20: OK"  << endl;
    } else {
        cout << "Test20: Fail" << endl;
    }

    HeaderPacket testHeader = {4040, 3131, 47351};
    if (memcmp(reinterpret_cast<char*>(&testHeader), sp.GetHeader(), sizeof(HeaderPacket)))
    {
        cout << "Test21: OK"  << endl;
    } else {
        cout << "Test21: Fail"  << endl;
    }

    return true;
}

bool thirdCheck(char* orig, int len)
{
    SmartPacket sp;
    sp.AllocateSend(TEST_PACKET_SIZE);
    HeaderPacket* head = sp.GetHeader();

    head->from = 4040;
    head->to = 3131;
    head->type = 47351;

    int step = len / 4;

    for (int i=0; i< len; i+=step)
    {
        char* tmp = orig + i;
        memcpy( sp.Position(), tmp, step); // sp.GetLength()
        sp.PositionMove(step);
    }


    if (checkPacket(orig, sp.GetData(), TEST_PACKET_SIZE))
    {
        cout << "Test30: OK"  << endl;
    } else {
        cout << "Test30: Fail" << endl;
    }

    HeaderPacket testHeader = {4040, 3131, 47351};

    if (memcmp(reinterpret_cast<char*>(&testHeader), sp.GetHeader(), sizeof(HeaderPacket)))
    {
        cout << "Test31: OK"  << endl;
    } else {
        cout << "Test31: Fail"  << endl;
    }

    return true;
}
