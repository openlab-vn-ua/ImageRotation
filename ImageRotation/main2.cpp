// **************************************************************
// NAME:            Fast Bitmap Rotation and Scaling Test
// AUTHORS:         Based on work by Steven M Mortimer,
//                  Extended By openlab.vn.ua team
// Target Platform: Win32
// ***************************************************************
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

#define _USE_MATH_DEFINES
#include <math.h>
double Math_PI = M_PI; // 3.1415;

/////////////////////////////////////////////////////////////////
// Globals
CDIB*   gDibSrc         = NULL;
CDIB*   gDibDst         = NULL;
float   gdScale         = 1.0 ; // _MAXSCALE;
float   gdAngle         = 0.0*Math_PI/180.0;
float   gdScaleDir      = 0.1f;
float   gdAngleStep     = Math_PI/180.0;
double  gdTicksPerSec   = 0.0;
bool    gbTimeFunction  = false;
bool    gbAutoMode      = false;
int     giViewZoomScale = 5; // Initial zoom scale
 
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

/// <summary>
/// Clips image data to region: [fromX, fromY].inclusive to (toX,toY).exclusive.
/// During clipping [fromX, fromY] and (toX,toY) region is adjusted (and returned), so it will be inside the image data.
/// Returns true if result is non-void region.
/// </summary>
/// <returns>true if result is non-empty (dstW > 0 and dstH > 0)</returns>
bool ClipImageProc(void *(&pDstBase), int pixelDataSize, int (&dstW), int (&dstH), int dstStride, int &fromX, int &fromY, int &toX, int &toY)
{
    if (dstW <= 0) { dstW = 0; }
    if (dstH <= 0) { dstH = 0; }

    if (fromX < 0) { fromX = 0; }
    if (fromY < 0) { fromY = 0; }

    if (toX <= fromX) { dstW = 0; }
    if (toY <= fromY) { dstH = 0; }

    if (fromX >= dstW) { dstW = 0; }
    if (fromY >= dstH) { dstH = 0; }

    if (toX > dstW) { toX = dstW; }
    if (toY > dstH) { toY = dstH; }

    if ((dstW <= 0) || (dstH <= 0))
    {
        return (false); // nothing to do
    }

    dstW = toX - fromX;
    dstH = toY - fromY;

    pDstBase = (void *)(((char*)pDstBase) + (fromX * pixelDataSize + fromY * dstStride));

    return(true);
}

/// <summary>
/// Clips image data to region: [fromX, fromY].inclusive to (toX,toY).exclusive.
/// During clipping [fromX, fromY] and (toX,toY) are adjusted, so it will be inside the image data.
/// Returns true if result is non-void region.
/// </summary>
/// <note> Same as ClipImageProc, except fromX, int fromY, int toX, int toY are not-references) </note>
/// <returns>true if result is non-empty (dstW > 0 and dstH > 0)</returns>
bool ClipImage(WDIBPIXEL *(&pDstBase), int (&dstW), int (&dstH), int dstStride, int fromX, int fromY, int toX, int toY)
{
    return(ClipImageProc((void*(&))pDstBase, sizeof(WDIBPIXEL), dstW, dstH, dstStride, fromX, fromY, toX, toY));
}

#define TEST_STEP_ANGLE_FORE()  { gdAngle += gdAngleStep; }
#define TEST_STEP_ANGLE_BACK()  { gdAngle -= gdAngleStep; }
#define TEST_STEP_SCALE_AUTO()  { if (gdScale <= _MINSCALE || gdScale >= _MAXSCALE) { gdScaleDir *= -1.0; } { gdScale += gdScaleDir*1.0f; } }
#define TEST_STEP_SCALE_FORE()  { if (gdScale < _MAXSCALE) { gdScale += fabs(gdScaleDir)*1.0f; } }
#define TEST_STEP_SCALE_BACK()  { if (gdScale > _MINSCALE) { gdScale -= fabs(gdScaleDir)*1.0f; } }

#define COLORREF_COLOR_BLACK    RGB(0, 0, 0)
#define COLORREF_COLOR_GOLD     RGB(0xFF, 0xD7, 0x00)

/////////////////////////////////////////////////////////////////
void Update(HDC hdc)
{
    double dStartT = GetTimer();
 
    ZeroMemory(gDibDst->m_pSrcBits, gDibDst->m_iSWidth * gDibDst->m_iHeight);

    // Prepare parameters
    WDIBPIXEL *pDstBase = gDibDst->m_pSrcBits; int dstW = gDibDst->m_iWidth; int dstH = gDibDst->m_iHeight; int dstDelta = gDibDst->m_iSWidth;
    WDIBPIXEL *pSrcBase = gDibSrc->m_pSrcBits; int srcW = gDibSrc->m_iWidth; int srcH = gDibSrc->m_iHeight; int srcDelta = gDibSrc->m_iSWidth;
    float fDstCX = dstW * 0.50 / giViewZoomScale + 5; float fDstCY = dstH * 0.50 / giViewZoomScale + 30;
    float fSrcCX = srcW * 0.00 + 0; float fSrcCY = srcH * 0.50 + 0;
    float fAngle = gdAngle;
    float fScale = gdScale;

    fDstCX = (int)fDstCX;
    fDstCY = (int)fDstCY;

    //ClipImage(pDstBase, dstW, dstH, dstDelta, 10, 75, 150, 190); // work this way also, so you may clip region before draw

    int TEST_COUNT = 10;

    for (int i = 0; i < TEST_COUNT; i++)
    {
        // Call Rotate routine
        // center of the source image as the points to rotate around
        RotateDrawWithClipAlt2
        //RotateDrawWithClipAltD
        //RotateDrawWithClipAlt
        //RotateDrawWithClip
        //RotateWrapFill
        (
            pDstBase, dstW, dstH, dstDelta,
            pSrcBase, srcW, srcH, srcDelta,
            fDstCX, fDstCY,
            fSrcCX, fSrcCY, 
            fAngle, fScale
        );
    }
     
    double dUpdateT = GetTimer();

    // Copy our rotated image to the screen (possible zoomed)

    if (giViewZoomScale <= 1)
    {
        BitBlt(hdc, 0, 0, gDibDst->m_iWidth, gDibDst->m_iHeight, gDibDst->m_hdc, 0, 0, SRCCOPY);
        SetPixel(hdc, fDstCX, fDstCY, COLORREF_COLOR_GOLD); // Draw center
    }
    else
    {
        int zoomWidth  = gDibDst->m_iWidth * giViewZoomScale;
        int zoomHeight = gDibDst->m_iHeight * giViewZoomScale;
        int fromWidth  = gDibDst->m_iWidth;
        int fromHeight = gDibDst->m_iHeight;

        auto res = 
        StretchDIBits
        (
            hdc,
            0,
            0,
            zoomWidth, 
            zoomHeight,
            0,
            0,
            fromWidth,
            fromHeight,
            gDibDst->m_pSrcBits,
            gDibDst->GetBMI(),
            gDibDst->GetDIBUsage(),
            SRCCOPY
        );

        int cx = fDstCX * giViewZoomScale;
        int cy = fDstCY * giViewZoomScale;

        RECT  center;
        
        SetRect(&center, cx, cy, cx+giViewZoomScale, cy+giViewZoomScale);
        FillRect(hdc, &center, (HBRUSH)GetStockObject(WHITE_BRUSH));

        SetPixel(hdc, cx, cy, COLORREF_COLOR_GOLD); // Draw center
    }

    double dRenderT = GetTimer();
 
    // Print function timing satistics
    if(gbTimeFunction)
    {
        int text_y_pos = 5;

        int TEXT_HEIGHT = 15;

        SetBkColor(hdc, COLORREF_COLOR_BLACK);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, COLORREF_COLOR_GOLD);

        char szBuffer[256];
        TextOut(hdc, 5, text_y_pos, szBuffer, 
             sprintf_s(szBuffer, "Rotate took %7.3fms (~%7.2ffps)",
             (dUpdateT-dStartT) * 1000.0 / TEST_COUNT, 1.0 / ((dUpdateT-dStartT) / TEST_COUNT)));
        text_y_pos += TEXT_HEIGHT;
        TextOut(hdc, 5, text_y_pos, szBuffer, 
             sprintf_s(szBuffer, "Render took %7.3fms (~%7.2ffps) [VIEW ZOOM: %d]",
             (dRenderT-dUpdateT) * 1000, 1.0 / (dRenderT-dUpdateT), giViewZoomScale));
        text_y_pos += TEXT_HEIGHT;
        TextOut(hdc, 5, text_y_pos, szBuffer, 
             sprintf_s(szBuffer, "Angle %3.6frad %4.1fdeg Scale %3.6f src X=%3.1f Y=%3.1f",
             (float)gdAngle, (float)(gdAngle/Math_PI*180.0), (float)gdScale, (float) fSrcCX, (float) fSrcCY));
        text_y_pos += TEXT_HEIGHT;
        TextOut(hdc, 5, text_y_pos, szBuffer, 
             sprintf_s(szBuffer, "Left/Right = Rotate, PgUp/PgDn = Scale+/-, A = Auto on/off, r=Reset"));
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

void DoRedraw(HWND hwnd)
{
    HDC hdc = GetDC(hwnd);
    Update(hdc);
    ReleaseDC(hwnd, hdc);
}

/////////////////////////////////////////////////////////////////
BOOL OnTimer(HWND hwnd, UINT id)
{   
    if (gbAutoMode)
    {
        TEST_STEP_SCALE_AUTO();
        DoRedraw(hwnd);
    }
    return TRUE;
}

void OnKeyDown(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
    switch (vk)
    {
        case(VK_ADD):
        case(VK_PRIOR): // PgUp
        {
            TEST_STEP_SCALE_FORE();
            DoRedraw(hwnd);
        } break;

        case(VK_SUBTRACT):
        case(VK_NEXT): // PgDn
        {
            TEST_STEP_SCALE_BACK();
            DoRedraw(hwnd);
        } break;

        case(VK_LEFT):
        {
            TEST_STEP_ANGLE_BACK();
            DoRedraw(hwnd);
        } break;

        case(VK_RIGHT):
        {
            TEST_STEP_ANGLE_FORE();
            DoRedraw(hwnd);
        } break;

        case('A'): // (VK_A):
        {
            gbAutoMode = !gbAutoMode;
            DoRedraw(hwnd);
        } break;

        case('R'):
        {
            gdScale    = 1.0 ; // _MAXSCALE;
            gdAngle    = 0.0*Math_PI/180.0;
            gdScaleDir = 0.1f;
            DoRedraw(hwnd);
        } break;

        default:
        {
            DoRedraw(hwnd);
        }
    }
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
        HANDLE_MSG( hwnd, WM_KEYDOWN,    OnKeyDown );
        HANDLE_MSG( hwnd, WM_DESTROY,    OnDestroy );
    }
 
    return DefWindowProc (hwnd, iMsg, wParam, lParam) ;
}
/////////////////////////////////////////////////////////////////
//End of File