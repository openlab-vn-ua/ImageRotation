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
#include "../src/rotate.h"

#define _USE_MATH_DEFINES
#include <math.h>
static double Math_PI = M_PI; // 3.1415;

// Utils
/////////////////////////////////////////////////////////////////

/// <summary>
/// Clips image data to region: [fromX, fromY].inclusive to (toX,toY).exclusive.
/// During clipping [fromX, fromY] and (toX,toY) region is adjusted (and returned), so it will be inside the image data.
/// Returns true if result is non-void region.
/// </summary>
/// <returns>true if result is non-empty (dstW > 0 and dstH > 0)</returns>
bool ClipImageProc(void* (&pDstBase), int pixelDataSize, int(&dstW), int(&dstH), int dstStride, int& fromX, int& fromY, int& toX, int& toY)
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

    pDstBase = (void*)(((char*)pDstBase) + (fromX * pixelDataSize + fromY * dstStride));

    return(true);
}

/// <summary>
/// Clips image data to region: [fromX, fromY].inclusive to (toX,toY).exclusive.
/// During clipping [fromX, fromY] and (toX,toY) are adjusted, so it will be inside the image data.
/// Returns true if result is non-void region.
/// </summary>
/// <note> Same as ClipImageProc, except fromX, int fromY, int toX, int toY are not-references) </note>
/// <returns>true if result is non-empty (dstW > 0 and dstH > 0)</returns>
bool ClipImage(RotatePixel_t* (&pDstBase), int(&dstW), int(&dstH), int dstStride, int fromX, int fromY, int toX, int toY)
{
    return(ClipImageProc((void* (&))pDstBase, sizeof(RotatePixel_t), dstW, dstH, dstStride, fromX, fromY, toX, toY));
}

#define COLORREF_COLOR_BLACK    RGB(0, 0, 0)
#define COLORREF_COLOR_GOLD     RGB(0xFF, 0xD7, 0x00)

// Hi speed timer
/////////////////////////////////////////////////////////////////

static bool    gbTimeFunction = false;
static double  gdTicksPerSec = 0.0;

static bool TimingInit()
{
    // Check if we can use the high performace counter
    // for timing the rotation function
    LARGE_INTEGER perfreq;
    if (QueryPerformanceFrequency(&perfreq))
    {
        gdTicksPerSec = (double)perfreq.LowPart;
        gbTimeFunction = true;
    }
    else
    {
        gbTimeFunction = false;
    }

    return(gbTimeFunction);
}

static bool TimingIsAvailable() { return gbTimeFunction; }

// Returns seconds since app start 
static double TimingGetValue() 
{
    if (gbTimeFunction)
    {
        LARGE_INTEGER time;
        QueryPerformanceCounter(&time);

        return (double)time.QuadPart / gdTicksPerSec;
    }
    return 0.0;
}


// Defines
/////////////////////////////////////////////////////////////////

#define SZIMAGE_1  "test_64x64.bmp"
#define SZIMAGE_2  "test_71x71.bmp"

// Global test state
/////////////////////////////////////////////////////////////////

CDIB*   gDibSrc         = NULL;
CDIB*   gDibDst         = NULL;

static const char* FUNC_NAME_RotateWrapFill = "RotateWrapFill";
static const char* FUNC_NAME_RotateDrawClip = "RotateDrawClip";
static const char* FUNC_NAME_RotateDrawClip1 = "RotateDrawClip1";
static const char* FUNC_NAME_RotateDrawClipExt1 = "RotateDrawClipExt";
static const char* FUNC_NAME_RotateDrawClipExt1D = "RotateDrawClipExt1D";
static const char* FUNC_NAME_RotateDrawClipExt2 = "RotateDrawClipExt2";

static const char* FUNC_NAMES[] = { FUNC_NAME_RotateWrapFill, FUNC_NAME_RotateDrawClip, FUNC_NAME_RotateDrawClip1, FUNC_NAME_RotateDrawClipExt1, FUNC_NAME_RotateDrawClipExt1D, FUNC_NAME_RotateDrawClipExt2 };
static const int   FUNC_NAMES_COUNT = sizeof(FUNC_NAMES) / sizeof(FUNC_NAMES[0]);

static const char* IMAGE_NAMES[] = { SZIMAGE_1, SZIMAGE_2 };
static const int   IMAGE_NAMES_COUNT = sizeof(IMAGE_NAMES) / sizeof(IMAGE_NAMES[0]);

#define GSCALE_MIN      0.4f
#define GSCALE_MAX      5.0f

float   gdScale         = 1.0 ;
float   gdAngle         = 0.0*Math_PI/180.0;
float   gdScaleDir      = 0.1f;
float   gdAngleStep     = Math_PI/180.0;
bool    gbAutoMode      = false;
int     giViewZoomScale = 5; // Initial zoom scale
int     giFuncNameIndex = 1; // FUNC_NAME_RotateDrawClip
int     giImageNameIndex= 0; // SZIMAGE_1

bool is_float_equal(float a, float b, float eps = 0.0001) { float d = a - b; if (d < 0) { d = -d; } return d <= eps; }

float norm_angle(float angle)
{
    if (is_float_equal(angle, 0)) { return 0; }
    if (is_float_equal(angle, Math_PI)) { return Math_PI; }
    if (is_float_equal(angle, 3*Math_PI / 2)) { return Math_PI / 2; }
    if (is_float_equal(angle, 3*Math_PI / 2)) { return 3*Math_PI / 2; }
    return angle;
}

#define GSTATE_STEP_ANGLE_FORE()  { gdAngle += gdAngleStep; gdAngle = norm_angle(gdAngle); }
#define GSTATE_STEP_ANGLE_BACK()  { gdAngle -= gdAngleStep; gdAngle = norm_angle(gdAngle); }
#define GSTATE_STEP_SCALE_AUTO()  { if (gdScale <= GSCALE_MIN || gdScale >= GSCALE_MAX) { gdScaleDir *= -1.0; } { gdScale += gdScaleDir*1.0f; } }
#define GSTATE_STEP_SCALE_FORE()  { if (gdScale < GSCALE_MAX) { gdScale += fabs(gdScaleDir)*1.0f; } }
#define GSTATE_STEP_SCALE_BACK()  { if (gdScale > GSCALE_MIN) { gdScale -= fabs(gdScaleDir)*1.0f; } }
#define GSTATE_STEP_FUNC_NAME()   { giFuncNameIndex++; giFuncNameIndex %= FUNC_NAMES_COUNT; }
#define GSTATE_STEP_IMAGE_NAME()  { giImageNameIndex++; giImageNameIndex %= IMAGE_NAMES_COUNT; gDibSrc->Load(IMAGE_NAMES[giImageNameIndex], sizeof(RotatePixel_t)); }
#define GSTATE_RESET()            { gdScale = 1.0; gdAngle = 0.0 * Math_PI / 180.0; gdScaleDir = 0.1f; }

/////////////////////////////////////////////////////////////////

static void Update(HDC hdc)
{
    ZeroMemory(gDibDst->m_pSrcBits, gDibDst->m_iSWidth * gDibDst->m_iHeight);

    // Prepare parameters
    RotatePixel_t *pDstBase = static_cast<RotatePixel_t*>(gDibDst->m_pSrcBits); int dstW = gDibDst->m_iWidth; int dstH = gDibDst->m_iHeight; int dstDelta = gDibDst->m_iSWidth;
    RotatePixel_t *pSrcBase = static_cast<RotatePixel_t*>(gDibSrc->m_pSrcBits); int srcW = gDibSrc->m_iWidth; int srcH = gDibSrc->m_iHeight; int srcDelta = gDibSrc->m_iSWidth;
    float fDstCX = dstW * 0.50 / giViewZoomScale + 5; float fDstCY = dstH * 0.50 / giViewZoomScale + 30;
    float fSrcCX = srcW * 0.00 + 0; float fSrcCY = srcH * 0.50 + 0;
    float fAngle = gdAngle;
    float fScale = gdScale;

    fDstCX = (int)fDstCX;
    fDstCY = (int)fDstCY;

    //ClipImage(pDstBase, dstW, dstH, dstDelta, 10, 75, 150, 190); // work this way also, so you may clip region before draw

    const char* FuncName = FUNC_NAMES[giFuncNameIndex];

    int iRotCount;

    #if defined(_DEBUG) || defined(DEBUG)
    iRotCount = 1000;
    #else
    iRotCount = 2000;
    #endif

    if (FuncName == FUNC_NAME_RotateDrawClip1)
    {
        iRotCount = iRotCount / 10;
    }
    else if (FuncName == FUNC_NAME_RotateWrapFill)
    {
        iRotCount = iRotCount / 20;
    }

    double dRotBeginT = TimingGetValue();

    for (int i = 0; i < iRotCount; i++)
    {
        if (FuncName == FUNC_NAME_RotateDrawClip)
        {
            RotateDrawClip
            (
                pDstBase, dstW, dstH, dstDelta,
                pSrcBase, srcW, srcH, srcDelta,
                fDstCX, fDstCY,
                fSrcCX, fSrcCY,
                fAngle, fScale
            );
        }
        else if (FuncName == FUNC_NAME_RotateDrawClip1)
        {
            RotateDrawClip1
            (
                pDstBase, dstW, dstH, dstDelta,
                pSrcBase, srcW, srcH, srcDelta,
                fDstCX, fDstCY,
                fSrcCX, fSrcCY,
                fAngle, fScale
            );
        }
        else if (FuncName == FUNC_NAME_RotateDrawClipExt1)
        {
            RotateDrawClipExt1
            (
                pDstBase, dstW, dstH, dstDelta,
                pSrcBase, srcW, srcH, srcDelta,
                fDstCX, fDstCY,
                fSrcCX, fSrcCY,
                fAngle, fScale
            );
        }
        else if (FuncName == FUNC_NAME_RotateDrawClipExt1D)
        {
            RotateDrawClipExt1D
            (
                pDstBase, dstW, dstH, dstDelta,
                pSrcBase, srcW, srcH, srcDelta,
                fDstCX, fDstCY,
                fSrcCX, fSrcCY,
                fAngle, fScale
            );
        }
        else if (FuncName == FUNC_NAME_RotateDrawClipExt2)
        {
            RotateDrawClipExt2
            (
                pDstBase, dstW, dstH, dstDelta,
                pSrcBase, srcW, srcH, srcDelta,
                fDstCX, fDstCY,
                fSrcCX, fSrcCY,
                fAngle, fScale
            );
        }
        else
        {
            RotateDrawFill
            (
                pDstBase, dstW, dstH, dstDelta,
                pSrcBase, srcW, srcH, srcDelta,
                fDstCX, fDstCY,
                fSrcCX, fSrcCY,
                fAngle, fScale
            );
        }
    }
     
    double dRotEndT = TimingGetValue();

    double fPixlesCount; // Pixels affected

    if (FuncName == FUNC_NAME_RotateWrapFill)
    {
        fPixlesCount = 1.0 * dstW * dstH;
    }
    else
    {
        fPixlesCount = 1.0 * srcW * srcH * fScale;
    }

    double dUpdateBeginT = TimingGetValue();

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

    double dUpdateEndT = TimingGetValue();
 
    // Print function timing satistics
    if(TimingIsAvailable())
    {
        int text_y_pos = 5;

        int TEXT_HEIGHT = 15;

        SetBkColor(hdc, COLORREF_COLOR_BLACK);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, COLORREF_COLOR_GOLD);

        char szBuffer[256];
        TextOut(hdc, 5, text_y_pos, szBuffer, 
             sprintf_s(szBuffer, "%s Rotate took %7.3fms (~%7.2ffps) (~%.1f MP/s)",
             FuncName, (dRotEndT-dRotBeginT) * 1000.0 / iRotCount, 1.0 / ((dRotEndT-dRotBeginT) / iRotCount), 1.0 * fPixlesCount * iRotCount / (dRotEndT - dRotBeginT) / 1e6));
        text_y_pos += TEXT_HEIGHT;
        TextOut(hdc, 5, text_y_pos, szBuffer, 
             sprintf_s(szBuffer, "Render took %7.3fms (~%7.2ffps) [VIEW ZOOM: %d]",
             (dUpdateEndT-dUpdateBeginT) * 1000, 1.0 / (dUpdateEndT-dUpdateBeginT), giViewZoomScale));
        text_y_pos += TEXT_HEIGHT;
        TextOut(hdc, 5, text_y_pos, szBuffer, 
             sprintf_s(szBuffer, "Angle %3.6frad %4.1fdeg Scale %3.6f src X=%3.1f Y=%3.1f Image=%s[%d x %d]",
             (float)gdAngle, (float)(gdAngle/Math_PI*180.0), (float)gdScale, (float) fSrcCX, (float) fSrcCY, IMAGE_NAMES[giImageNameIndex], srcW, srcH));
        text_y_pos += TEXT_HEIGHT;
        TextOut(hdc, 5, text_y_pos, szBuffer, 
             sprintf_s(szBuffer, "Left/Right=Rotate, PgUp/PgDn=Scale+/-, A=Auto(on/off), r=Reset, f=Func, i=Image"));
    }
}

static BOOL OnCreate(HWND hwnd, CREATESTRUCT FAR* lpCreateStruct)
{
    const char* SZIMAGE = IMAGE_NAMES[giImageNameIndex];

    BOOL bSuccess = gDibSrc->Load(SZIMAGE, sizeof(RotatePixel_t));
    if (bSuccess)
    {
        // Start the update timer
        SetTimer(hwnd, 0, 100, NULL);
    }   
    else
    {
        char szError[512];
        wsprintf(szError, "Error loading image %s", SZIMAGE);
        MessageBox(hwnd, szError, "oops", MB_OK);
    }

    return bSuccess;
}

static void OnSize(HWND hwnd, UINT state, int x, int y)
{
    // Recreate the window DIB to match the size of the window
    gDibDst->Create(NULL, 0, 0, x, y, sizeof(RotatePixel_t));
}

static BOOL OnEraseBkGnd(HWND hwnd, HDC hdc)
{   
    // clearing the bk results in tears.
    return TRUE;
}

static void OnPaint(HWND hwnd)
{
    HDC hdc;
    PAINTSTRUCT ps;
 
    hdc = BeginPaint (hwnd, &ps);
    Update(hdc);
    EndPaint (hwnd, &ps) ;
}

static void DoRedraw(HWND hwnd)
{
    HDC hdc = GetDC(hwnd);
    Update(hdc);
    ReleaseDC(hwnd, hdc);
}

static BOOL OnTimer(HWND hwnd, UINT id)
{   
    if (gbAutoMode)
    {
        GSTATE_STEP_SCALE_AUTO();
        DoRedraw(hwnd);
    }
    return TRUE;
}

static void OnKeyDown(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
    switch (vk)
    {
        case(VK_ADD):
        case(VK_PRIOR): // PgUp
        {
            GSTATE_STEP_SCALE_FORE();
            DoRedraw(hwnd);
        } break;

        case(VK_SUBTRACT):
        case(VK_NEXT): // PgDn
        {
            GSTATE_STEP_SCALE_BACK();
            DoRedraw(hwnd);
        } break;

        case(VK_LEFT):
        {
            GSTATE_STEP_ANGLE_BACK();
            DoRedraw(hwnd);
        } break;

        case(VK_RIGHT):
        {
            GSTATE_STEP_ANGLE_FORE();
            DoRedraw(hwnd);
        } break;

        case('A'): // (VK_A):
        {
            gbAutoMode = !gbAutoMode;
            DoRedraw(hwnd);
        } break;

        case('R'):
        {
            GSTATE_RESET();
            DoRedraw(hwnd);
        } break;

        case('F'):
        {
            GSTATE_STEP_FUNC_NAME();
            DoRedraw(hwnd);
        } break;

        case('I'):
        {
            GSTATE_STEP_IMAGE_NAME();
            DoRedraw(hwnd);
        } break;

        default:
        {
            DoRedraw(hwnd);
        }
    }
}

static void OnDestroy(HWND hwnd)
{
    PostQuitMessage(0);
}

/////////////////////////////////////////////////////////////////

static LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{

    HWND        hwnd;
    MSG         msg;

    // Register the window class
    WNDCLASS wndclass;
    ZeroMemory(&wndclass, sizeof(WNDCLASS));
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.hInstance = hInstance;
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.lpszClassName = __FILE__;
    RegisterClass(&wndclass);

    if (!TimingInit())
    {
        MessageBox(NULL,
            "High resolution timer not available",
            "Warning", MB_OK);
    }

    // Create our source and destination DIB`s
    gDibSrc = new CDIB();
    gDibDst = new CDIB();

    if (gDibSrc && gDibDst)
    {
        // Create Window
        hwnd = CreateWindow(
            __FILE__, "Fast Bitmap Rotation", WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 400, 400,
            NULL, NULL, hInstance, NULL);

        ShowWindow(hwnd, iCmdShow);
        UpdateWindow(hwnd);

        while (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // cleanup DIB`s
        delete gDibSrc;
        delete gDibDst;
    }
    return msg.wParam;
}

