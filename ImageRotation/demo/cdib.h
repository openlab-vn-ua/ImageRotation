#ifndef CDLIB_H_INCLUDED
#define CDLIB_H_INCLUDED

#include <windows.h>

///////////////////////////////////////////////////////////////////
class CDIB
{
    public:
    CDIB();
    ~CDIB();
 
    bool            Create(HDC hdcSrc, int iSrcX, int iSrcY, int iWidth, int iHeight, int bytesPerPixel);
    void            Release();
 
    public:

    // Varibles
    HDC             m_hdc;          // HDC of the DIB
    HBITMAP         m_hbm;          // HBITMAP of the DIB   
    HBITMAP         m_hbmOld;       // old HBITMAP from the hdc
    void*           m_pSrcBits;     // pointer to DIB pixel array
    int             m_iWidth;       // Width of the DIB
    int             m_iHeight;      // Height of the DIB
    int             m_iSWidth;      // Storage Width (in bytes)

    protected:
    BITMAPINFO      bmi;

    public:
    BITMAPINFO     *GetBMI()         { return(&bmi); }
    UINT            GetDIBUsage()    { return(DIB_RGB_COLORS); }
};

#endif
