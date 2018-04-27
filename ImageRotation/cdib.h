#ifndef CDLIB_H_INCLUDED
#define CDLIB_H_INCLUDED

#if !defined(WDIBPIXEL_BITS)
#include <stdint.h>
typedef uint16_t WDIBPIXEL;
#define WDIBPIXEL_BITS (sizeof(WDIBPIXEL)*8)
#endif

#include <windows.h>

///////////////////////////////////////////////////////////////////
class CDIB
{
    public:
    CDIB();
    ~CDIB();
 
    bool            Create(HDC hdcSrc, int iSrcX, int iSrcY, 
                                int iWidth, int iHeight);
    void            Release();
 
    // Varibles
    HDC             m_hdc;          // HDC of the DIB
    HBITMAP         m_hbm;          // HBITMAP of the DIB   
    HBITMAP         m_hbmOld;       // old HBITMAP from the hdc
    WDIBPIXEL*      m_pSrcBits;     // pointer to DIB pixel array
    int             m_iWidth;       // Width of the DIB
    int             m_iHeight;      // Height of the DIB
    int             m_iSWidth;      // Storage Width (in bytes)
};

#endif
