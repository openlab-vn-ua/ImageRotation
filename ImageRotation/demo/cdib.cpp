//////////////////////////////////////////////////////////////////
// CDIB - Small wrapper class to handle DIB`s
 
#include "cdib.h"
#include <windows.h>

#define ROUND_UP(VAL, MOD) ((((VAL) + (MOD) - 1) / (MOD)) * (MOD))
 
//////////////////////////////////////////////////////////////////
CDIB::CDIB()
{
    m_hdc = 0;
    m_hbm = 0;
    m_hbmOld = 0;
    m_pSrcBits = NULL;
    m_iWidth = 0;
    m_iHeight = 0;
    m_iSWidth = 0;
}
//////////////////////////////////////////////////////////////////
CDIB::~CDIB()
{
    Release();
}
 
//////////////////////////////////////////////////////////////////
// Takes DC handle, creates a DIB from given location and size
bool CDIB::Create(HDC hdcSrc, int iSrcX, int iSrcY,
                  int iWidth, int iHeight)
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
    ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize            =   sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth           =   iWidth;
    bmi.bmiHeader.biHeight          =   iInitHeight;
    bmi.bmiHeader.biPlanes          =   1;
    bmi.bmiHeader.biBitCount        =   WDIBPIXEL_BITS;
    bmi.bmiHeader.biCompression     =   BI_RGB;
    bmi.bmiHeader.biXPelsPerMeter   =   72;
    bmi.bmiHeader.biYPelsPerMeter   =   72;
 
    m_iWidth = iWidth;
    m_iHeight = iHeight;
     
    // Each line of the DIB is always quad aligned.
    m_iSWidth = ROUND_UP(iWidth*sizeof(WDIBPIXEL), 4); 
 
    // Get hdc of screen for CreateDIBSection and create the DIB
    HDC hdcScreen = GetDC(NULL);
    m_hbm = CreateDIBSection(
                hdcScreen, &bmi, GetDIBUsage(), 
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
//////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////
//End of File
