///////////////////////////////////////////////////////////////////
void Rotate(
    WORD *pDstBase, int dstW, int dstH, int dstDelta,
    WORD *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstRotCenterX, float fDstRotCenterY,
    float fSrcRotCenterX, float fSrcRotCenterY, 
    float fAngle, float fScale);
 
///////////////////////////////////////////////////////////////////
void FastRotate(
    WORD *pDstBase, int dstW, int dstH, int dstDelta,
    WORD *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstRotCenterX, float fDstRotCenterY,
    float fSrcRotCenterX, float fSrcRotCenterY, 
    float fAngle, float fScale);
 
///////////////////////////////////////////////////////////////////
void RotateWithClip(
    WORD *pDstBase, int dstW, int dstH, int dstDelta,
    WORD *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstRotCenterX, float fDstRotCenterY,
    float fSrcRotCenterX, float fSrcRotCenterY, 
    float fAngle, float fScale);
/* End of File */
