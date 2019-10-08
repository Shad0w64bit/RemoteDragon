#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <iostream>
#include "resource.h"

#include "../../include/RemoteDragon2.h"
#include "later.h"

HINSTANCE hInst;
RemoteDragon2 rd;
HBITMAP bmp;
bool event_exit = false;
HWND h;

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
//            std::cout << n << std::endl;
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
//        rd.SendScreen();
        h = hwnd;
/*        bmp = mm.CaptureScreen(2);
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        DeleteObject(bmp);*/
    }
//    UpdateWindow(hwnd);
}

void ReciveScreen(const char* buffer, int len)
{
/*    BITMAPINFOHEADER   bmih;
	memset(&bmih, 0, sizeof(BITMAPINFOHEADER));

	bmih.biWidth = 2560;
	bmih.biHeight = 1440;
	bmih.biBitCount = 32;
	bmih.biCompression = BI_RGB;
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biPlanes = 1;

	BITMAPINFO* bmi = (BITMAPINFO*)&bmih;
//	HDC hdc = CreateCompatibleDC(GetDC(NULL));


	    PAINTSTRUCT ps;
        BITMAP bitmap;
        HDC hdcMem;
        HGDIOBJ oldBitmap;

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

    RedrawWindow(h, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);*/
//    DeleteObject(bmp);
//    std::cout << "Recive Screen " << len << std::endl;

}


BOOL CALLBACK DlgMain(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_INITDIALOG:
    {
        rd.Connect("127.0.0.1", 22222);
        rd.SetReciveMsgFunc(ReciveMsg);
//        rd.SetReciveScreenFunc(ReciveScreen);
        char buf [20];
        for (int i=0; i<100; i++)
        {
            sprintf(buf, "Hello World %d!", i);
            rd.SendMsg(buf);
        }

        //rd.SendScreen();

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

    case WM_PAINT:
    {
/*        PAINTSTRUCT ps;
        HDC hdc;
        BITMAP bitmap;
        HDC hdcMem;
        HGDIOBJ oldBitmap;

        hdc = BeginPaint(hwndDlg, &ps);
        hdcMem = CreateCompatibleDC(hdc);
        oldBitmap = SelectObject(hdcMem, bmp);
        GetObject(bmp, sizeof(bitmap), &bitmap);
        BitBlt(hdc, 0,0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

        SelectObject(hdcMem, oldBitmap);
        DeleteDC(hdcMem);
        EndPaint(hwndDlg, &ps);*/
    }
    return TRUE;

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
