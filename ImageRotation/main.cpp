/* **************************************************************
** NAME:            Fast Bitmap Rotation and Scaling
** AUTHOR:          Steven M Mortimer
** COMPILER:        Visual C++ V6.0 Enterprise SP3
** Target Platform: Win32
** RIGHTS:          Stevem Mortimer
** Date:            18th Jan 2000
************************************************************** */
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include "cdib.h"
#include "rotate.h"
 
/////////////////////////////////////////////////////////////////
// defines
#define _MINSCALE   0.4f
#define _MAXSCALE   5.0f
#define SZIMAGE     "test1.bmp"
 
/////////////////////////////////////////////////////////////////
// Globals
CDIB*   gDibSrc         = NULL;
CDIB*   gDibDst         = NULL;
float   gdScale         = _MAXSCALE;
float   gdAngle         = 0.0f;
float   gdScaleDir      = 0.1f;
double  gdTicksPerSec   = 0.0;
bool    gbTimeFunction  = false;
 
 
/////////////////////////////////////////////////////////////////
// Function Prototypes
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
 
/////////////////////////////////////////////////////////////////
// WinMain
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR szCmdLine, int iCmdShow)
{
 
    HWND        hwnd;
    MSG         msg;    
 
    // Register the window class
    WNDCLASS wndclass;
    ZeroMemory(&wndclass, sizeof(WNDCLASS));
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc;
    wndclass.hInstance     = hInstance;
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.lpszClassName = __FILE__;
    RegisterClass(&wndclass);
 
    // Check if we can use the high performace counter
    // for timing the rotation function
    LARGE_INTEGER perfreq;
    if(QueryPerformanceFrequency(&perfreq))
    {
        gdTicksPerSec = (double)perfreq.LowPart;
        gbTimeFunction = true;
    }
    else
    {
        MessageBox(NULL, 
            "High resolution timer not available",
            "Warning", MB_OK);
    }
         
    // Create our source and destination DIB`s
    gDibSrc = new CDIB();
    gDibDst = new CDIB();
 
    if(gDibSrc && gDibDst)
    {
        // Create Window
        hwnd = CreateWindow(
           __FILE__, "Fast Bitmap Rotation", WS_OVERLAPPEDWINDOW,
           CW_USEDEFAULT, CW_USEDEFAULT, 400, 400, 
           NULL, NULL, hInstance, NULL);
 
        ShowWindow (hwnd, iCmdShow) ;
        UpdateWindow (hwnd) ;
 
        while (GetMessage (&msg, NULL, 0, 0))
        {
            TranslateMessage (&msg) ;
            DispatchMessage (&msg) ;
        }       
 
        // cleanup DIB`s
        delete gDibSrc;
        delete gDibDst;
    }
    return msg.wParam;
}
/////////////////////////////////////////////////////////////////
double GetTimer()
{
    if(gbTimeFunction)
    {
        LARGE_INTEGER time;
        QueryPerformanceCounter(&time);
 
        return (double)time.QuadPart / gdTicksPerSec;
    }
    return 0.0;
}
/////////////////////////////////////////////////////////////////
void Update(HDC hdc)
{
    double dStartT = GetTimer();
 
    // Call Rotate routine, using center of the window and the
    // center of the source image as the points to rotate around
    FastRotate(
        gDibDst->m_pSrcBits, gDibDst->m_iWidth, 
        gDibDst->m_iHeight, gDibDst->m_iSWidth,
        gDibSrc->m_pSrcBits, gDibSrc->m_iWidth, 
        gDibSrc->m_iHeight, gDibSrc->m_iSWidth,
        (float)gDibDst->m_iWidth/2, (float)gDibDst->m_iHeight/2,
        (float)gDibSrc->m_iWidth/2, (float)gDibSrc->m_iHeight/2,
        gdAngle, gdScale);
     
    // Change direction of the scale
    if(gdScale <= _MINSCALE || gdScale >= _MAXSCALE)
    {
        gdScaleDir *= -1.0;
    }
 
    // Update angle and scale
    gdScale += gdScaleDir;
    gdAngle += 0.02f;
     
    double dUpdateT = GetTimer();
 
    // Copy our rotated image to the screen
    BitBlt(hdc, 0, 0, gDibDst->m_iWidth, gDibDst->m_iHeight, 
            gDibDst->m_hdc, 0, 0, SRCCOPY);
     
    double dRenderT = GetTimer();
 
    // Print function timing satistics
    if(gbTimeFunction)
    {
        char szBuffer[256];
        TextOut(hdc, 5, 5, szBuffer, 
             sprintf(szBuffer, "Update took %3.6f (~%3.2ffps)",
             dUpdateT-dStartT, 1.0 / (dUpdateT-dStartT)));
        TextOut(hdc, 5, 20, szBuffer, 
             sprintf(szBuffer, "Render took %3.6f (~%3.2ffps)",
             dRenderT-dUpdateT, 1.0 / (dRenderT-dUpdateT)));
    }
}
////////////////////////////////////////////////////////////////
BOOL OnCreate(HWND hwnd, CREATESTRUCT FAR* lpCreateStruct)
{   
    BOOL bSuccess = FALSE;
 
    // Load test bitmap
    HBITMAP hbm = (HBITMAP)LoadImage(NULL, SZIMAGE, IMAGE_BITMAP,
                                     0, 0, LR_LOADFROMFILE);
 
    if(hbm)
    {
        // Map the bitmap into a dc
        HDC hdc = CreateCompatibleDC(NULL);
        HBITMAP hbmOld = (HBITMAP)SelectObject(hdc, hbm);
         
        // Get info about this bitmap       
        BITMAP bm;      
        if(GetObject(hbm, sizeof(BITMAP), &bm) != 0)
        {
            // Convert the bitmap into DIB of known colour depth
            if(gDibSrc->Create(hdc, 0, 0, 
                bm.bmWidth, bm.bmHeight))
            {       
                // Start the update timer
                SetTimer(hwnd, 0, 100, NULL);
                bSuccess = TRUE;
            }
             
            // cleanup hdc
            SelectObject(hdc, hbmOld);
            DeleteDC(hdc);
        }
        // delete the loaded image
        DeleteObject(hbm);
    }   
    else
    {
        char szError[512];
        wsprintf(szError, "Error loading image %s", SZIMAGE);
        MessageBox(hwnd, szError, "oops", MB_OK);
    }
    return bSuccess;
}
/////////////////////////////////////////////////////////////////
void OnSize(HWND hwnd, UINT state, int x, int y)
{
    // Recreate the window DIB to match the size of the window
    gDibDst->Create(NULL, 0, 0, x, y);  
}
/////////////////////////////////////////////////////////////////
BOOL OnEraseBkGnd(HWND hwnd, HDC hdc)
{   
    // clearing the bk results in tears.
    return TRUE;
}
/////////////////////////////////////////////////////////////////
void OnPaint(HWND hwnd)
{
    HDC hdc;
    PAINTSTRUCT ps;
 
    hdc = BeginPaint (hwnd, &ps);
    Update(hdc);
    EndPaint (hwnd, &ps) ;
}
 
/////////////////////////////////////////////////////////////////
BOOL OnTimer(HWND hwnd, UINT id)
{   
    HDC hdc = GetDC(hwnd);
    Update(hdc);
    ReleaseDC(hwnd, hdc);
    return TRUE;
}
/////////////////////////////////////////////////////////////////
void OnDestroy(HWND hwnd)
{
    PostQuitMessage(0);
}
/////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, 
                            WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)
    {
        HANDLE_MSG( hwnd, WM_CREATE,     OnCreate );
        HANDLE_MSG( hwnd, WM_SIZE,       OnSize );
        HANDLE_MSG( hwnd, WM_PAINT,      OnPaint );
        HANDLE_MSG( hwnd, WM_ERASEBKGND, OnEraseBkGnd );
        HANDLE_MSG( hwnd, WM_TIMER,      OnTimer );
        HANDLE_MSG( hwnd, WM_DESTROY,    OnDestroy );
    }
 
    return DefWindowProc (hwnd, iMsg, wParam, lParam) ;
}
/////////////////////////////////////////////////////////////////
//End of File
