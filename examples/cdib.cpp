#include "cdib.h"
#include <windows.h>

#define ROUND_UP(VAL, MOD) ((((VAL) + (MOD) - 1) / (MOD)) * (MOD))
 
CDIB::CDIB()
{
    m_hdc = 0;
    m_hbm = 0;
    m_hbmOld = 0;
    m_pSrcBits = NULL;
    m_iWidth = 0;
    m_iHeight = 0;
    m_iSWidth = 0;

    ZeroMemory(&m_bmi, sizeof(m_bmi));
}

CDIB::~CDIB()
{
    Release();
}
 
bool CDIB::Create(HDC hdcSrc, int iSrcX, int iSrcY, int iWidth, int iHeight, int iBytesPerPixel)
{
    // Release the old DIB
    Release();

    #if defined(CDLIB_BITMAP_BOTTOM_TOP)
    int iInitHeight = iHeight;
    #else
    int iInitHeight = -iHeight;
    #endif
 
    // fill in the BITMAPINFO structure
    //BITMAPINFO bmi;
    ZeroMemory(&m_bmi, sizeof(m_bmi));
    m_bmi.bmiHeader.biSize            =   sizeof(BITMAPINFOHEADER);
    m_bmi.bmiHeader.biWidth           =   iWidth;
    m_bmi.bmiHeader.biHeight          =   iInitHeight;
    m_bmi.bmiHeader.biPlanes          =   1;
    m_bmi.bmiHeader.biBitCount        =   iBytesPerPixel * 8;
    m_bmi.bmiHeader.biCompression     =   BI_RGB;
    m_bmi.bmiHeader.biXPelsPerMeter   =   72;
    m_bmi.bmiHeader.biYPelsPerMeter   =   72;
 
    m_iWidth = iWidth;
    m_iHeight = iHeight;
     
    // Each line of the DIB is always quad aligned.
    m_iSWidth = ROUND_UP(iWidth*iBytesPerPixel, 4);
 
    // Get hdc of screen for CreateDIBSection and create the DIB
    HDC hdcScreen = GetDC(NULL);
    m_hbm = CreateDIBSection(
                hdcScreen, &m_bmi, GetDIBUsage(), 
                (void**)&m_pSrcBits, NULL, 0);
 
    // Create and select the bitmap into a DC
    m_hdc = CreateCompatibleDC(hdcScreen);        
    m_hbmOld = (HBITMAP)SelectObject(m_hdc, m_hbm);
 
    // Copy the original image into the DIB
    BitBlt(m_hdc, 0, 0, iWidth, iHeight, hdcSrc,
           iSrcX, iSrcY, SRCCOPY);
 
    // all GDI functions must be flushed before we can use the DIB
    GdiFlush();
 
    ReleaseDC(NULL, hdcScreen);
    return true;
}

bool CDIB::Load(const char* szFileName, int bytesPerPixel)
{
    bool bSuccess = false;

    HBITMAP hbm = (HBITMAP)LoadImage(NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    if (hbm)
    {
        // Map the bitmap into a dc
        HDC hdc = CreateCompatibleDC(NULL);
        HBITMAP hbmOld = (HBITMAP)SelectObject(hdc, hbm);

        // Get info about this bitmap       
        BITMAP bm;
        if (GetObject(hbm, sizeof(BITMAP), &bm) != 0)
        {
            // Convert the bitmap into DIB of known colour depth
            if (this->Create(hdc, 0, 0, bm.bmWidth, bm.bmHeight, bytesPerPixel))
            {
                bSuccess = true;
            }

            // cleanup hdc
            SelectObject(hdc, hbmOld);
            DeleteDC(hdc);
        }

        // delete the loaded image
        DeleteObject(hbm);
    }

    return (bSuccess);
}

void CDIB::Release()
{
    if(m_hdc)
    {
        if(m_hbmOld)
        {
            SelectObject(m_hdc, m_hbmOld);
        }

        if(m_hbm)
        {
            DeleteObject(m_hbm);
        }

        DeleteDC(m_hdc);
    }

    m_hdc = NULL;
    m_hbm = NULL;
    m_hbmOld = NULL;
    m_pSrcBits = NULL;
    m_iWidth = 0;
    m_iHeight = 0;
}
