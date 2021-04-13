#ifndef CDLIB_H_INCLUDED
#define CDLIB_H_INCLUDED

#include <windows.h>

// CDIB - Small wrapper class to handle DIB`s
class CDIB
{
    public:
    CDIB();
    ~CDIB();
 
    // Takes DC handle, creates a DIB from given location and size
    bool            Create(HDC hdcSrc, int iSrcX, int iSrcY, int iWidth, int iHeight, int bytesPerPixel);

    // Load bitmap from file and cobvert it to specified color depth
    bool            Load(const char* szFileName, int bytesPerPixel);

    // Release all linked resources
    void            Release();
 
    // #region Varibles

    public:

    HDC             m_hdc;          // HDC of the DIB
    HBITMAP         m_hbm;          // HBITMAP of the DIB   
    HBITMAP         m_hbmOld;       // old HBITMAP from the hdc
    void*           m_pSrcBits;     // pointer to DIB pixel array
    int             m_iWidth;       // Width of the DIB
    int             m_iHeight;      // Height of the DIB
    int             m_iSWidth;      // Storage Width (in bytes)

    // #endregion

    protected:
    BITMAPINFO      m_bmi;

    public:

    // Get BITMAPINFO of current structure
    BITMAPINFO     *GetBMI()         { return(&m_bmi); }

    // Get current DIB Usage (AKA "palette mode")
    UINT            GetDIBUsage()    { return(DIB_RGB_COLORS); }
};

#endif
