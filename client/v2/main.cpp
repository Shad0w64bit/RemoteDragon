#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <atomic>
#include <iostream>
#include "resource.h"

#include "../../include/RemoteDragon2.h"
#include "later.h"

HINSTANCE hInst;
RemoteDragon2 rd;
HBITMAP bmp;
const char* iData;
int iLen;
bool event_exit = false;
HWND h;
std::atomic<bool> refresh;

void ReciveMsg(const char* str)
{
    std::cout << str << std::endl;
}

void updateImage(HWND hwnd)
{
//    later later_update(1, true, &updateImage, hwnd);
    while (!event_exit) {
        static int t = 0;
        static unsigned int n = 0;
        int now = GetTickCount();
        if ((t+1000) < now) {
            //std::cout << n << std::endl;
            t = now;
            n = 0;
        } else {
            n++;
        }

/*        char buf [20];
        for (int i=0; i<10; i++)
        {
            sprintf(buf, "Hello World %d!", i);
            rd.SendMsg(buf);
        }
*/
//        rd.Refresh();
        rd.SendScreen();
/*        h = hwnd;
        bmp = mm.CaptureScreen(2);
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        DeleteObject(bmp);*/
        //Sleep(50);
    }
    UpdateWindow(hwnd);
}

void ReciveScreen(const char* buffer, int len)
{
/*    iData = buffer;
    iLen = len;*/

/*    BITMAPINFO bif;
    ZeroMemory(&bif, sizeof(BITMAPINFO));

    bif.bmiHeader.biSize = sizeof(bif);

    bif.bmiHeader.biHeight = 1440;
    bif.bmiHeader.biWidth = 2560;
    bif.bmiHeader.biPlanes = 1;
    bif.bmiHeader.biBitCount = 24;
    bif.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bif.bmiHeader.biCompression = BI_RGB;

//    HDC hdc = CreateCompatibleDC(GetDC(NULL));
    PAINTSTRUCT ps;

    HDC hdc = BeginPaint(h, &ps);
    SetDIBitsToDevice(hdc, 0, 0, 2560, 1440, 0, 0, 0, 2560, buffer, &bif, DIB_RGB_COLORS);
    EndPaint(h, &ps);*/

    BITMAPINFOHEADER   bmih;
	memset(&bmih, 0, sizeof(BITMAPINFOHEADER));

	bmih.biWidth = rd.GetWidth(); //2560
	bmih.biHeight = rd.GetHeight(); // 1440
	bmih.biBitCount = 24;
	bmih.biCompression = BI_RGB;
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biPlanes = 1;

	BITMAPINFO* bmi = (BITMAPINFO*)&bmih;
//	HDC hdc = CreateCompatibleDC(GetDC(NULL));


	    PAINTSTRUCT ps;
        BITMAP bitmap;
        HDC hdcMem;
        HGDIOBJ oldBitmap;
//refresh = true;
        HDC hdc = BeginPaint(h, &ps);
        hdcMem = CreateCompatibleDC(hdc);
        bmp = CreateDIBitmap(hdc, &bmih, CBM_INIT, buffer, bmi, DIB_RGB_COLORS);
        oldBitmap = SelectObject(hdcMem, bmp);
        GetObject(bmp, sizeof(bitmap), &bitmap);
        BitBlt(hdc, 0,0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

//        DeleteObject(bmp);


        SelectObject(hdcMem, oldBitmap);
        DeleteDC(hdcMem);
        EndPaint(h, &ps);
  //      refresh = false;

    // InvalidateRect(h, NULL, true);

    RedrawWindow(h, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
//    DeleteObject(bmp);
    std::cout << "Recive Screen " << len << std::endl;

}


BOOL CALLBACK DlgMain(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_INITDIALOG:
    {
        rd.Connect("127.0.0.1", 22222);
        rd.SetReciveMsgFunc(ReciveMsg);
        rd.SetReciveScreenFunc(ReciveScreen);
        h = hwndDlg;
        rd.SendResolution();
/*        char buf [20];
        for (int i=0; i<100; i++)
        {
            sprintf(buf, "Hello World %d!", i);
            rd.SendMsg(buf);
        }*/

//        rd.SendScreen();

//
        later later_update(1, true, &updateImage, hwndDlg);
    }
    return TRUE;

    case WM_CLOSE:
    {
        rd.Disconnect();
        event_exit = true;
        EndDialog(hwndDlg, 0);
    }
    return TRUE;

    case WM_DISPLAYCHANGE:
    {
        rd.SendResolution();
    }
    return TRUE;

    case WM_PAINT:
    {
        /*PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwndDlg, &ps);
        HDC hdcMem = CreateCompatibleDC(hdc);
        //HBITMAP oldBitmap = SelectObject(hdcMem, bmp)


        DeleteDC(hdcMem);

        EndPaint(hwndDlg, &ps);*/
    }

   /* {
        PAINTSTRUCT ps;
        HDC hdc;
        BITMAP bitmap;
        HDC hdcMem;
        HGDIOBJ oldBitmap;

        refresh = true;
        hdc = BeginPaint(hwndDlg, &ps);
        hdcMem = CreateCompatibleDC(hdc);
        oldBitmap = SelectObject(hdcMem, bmp);
        GetObject(bmp, sizeof(bitmap), &bitmap);
        BitBlt(hdc, 0,0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

        SelectObject(hdcMem, oldBitmap);
        DeleteDC(hdcMem);
        EndPaint(hwndDlg, &ps);
        refresh = false;
    }
    return TRUE;*/

    case WM_COMMAND:
    {
        switch(LOWORD(wParam))
        {
        }
    }

    return TRUE;
    }
    return FALSE;
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    hInst=hInstance;
    InitCommonControls();
    return DialogBox(hInst, MAKEINTRESOURCE(DLG_MAIN), NULL, (DLGPROC)DlgMain);
}
