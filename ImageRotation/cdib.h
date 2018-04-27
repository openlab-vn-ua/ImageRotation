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
    WORD*           m_pSrcBits;     // pointer to DIB pixel array
    int             m_iWidth;       // Width of the DIB
    int             m_iHeight;      // Height of the DIB
    int             m_iSWidth;      // Storage Width (in bytes)
};
/* End of File */