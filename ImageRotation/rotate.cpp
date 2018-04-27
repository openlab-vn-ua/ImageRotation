#include <windows.h>
#include <math.h>
 
//////////////////////////////////////////////////////////////////
// Rotate - wrapping version
// This version takes any dimension source bitmap and wraps.
//////////////////////////////////////////////////////////////////
void Rotate(
    WORD *pDstBase, int dstW, int dstH, int dstDelta,
    WORD *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstCX, float fDstCY,
    float fSrcCX, float fSrcCY, 
    float fAngle, float fScale)
{   
 
    srcDelta /= sizeof(WORD);
    dstDelta /= sizeof(WORD);
 
    float duCol = (float)sin(-fAngle) * (1.0f / fScale);
    float dvCol = (float)cos(-fAngle) * (1.0f / fScale);
    float duRow = dvCol;
    float dvRow = -duCol;
 
    float startingu = fSrcCX - (fDstCX * dvCol + fDstCY * duCol);
    float startingv = fSrcCY - (fDstCX * dvRow + fDstCY * duRow);
 
    float rowu = startingu;
    float rowv = startingv;
 
    for(int y = 0; y < dstH; y++)
    {
        float u = rowu;
        float v = rowv;
     
        WORD *pDst = pDstBase + (dstDelta * y);
 
        for(int x = 0; x < dstW ; x++)
        {   
            int sx = ((u < 0.0 ? (int)-u : (int)u) + srcW) % srcW;
            int sy = ((v < 0.0 ? (int)-v : (int)v) + srcH) % srcH;
 
            WORD *pSrc = pSrcBase + sx + (sy * srcDelta);
                         
            *pDst++ = *pSrc++;
 
            u += duRow;
            v += dvRow;
        }
 
        rowu += duCol;
        rowv += dvCol;
    }
}
//////////////////////////////////////////////////////////////////
// FastRotate - wrapping version
//
// This version assumes the dimensions of the source image to be a 
// power of two. For none-power2 dimensions, use Rotate
//
//////////////////////////////////////////////////////////////////
 
void FastRotate(
    WORD *pDstBase, int dstW, int dstH, int dstDelta,
    WORD *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstCX, float fDstCY,
    float fSrcCX, float fSrcCY, 
    float fAngle, float fScale)
{   
    srcDelta /= sizeof(WORD);
    dstDelta /= sizeof(WORD);
 
    float duCol = (float)sin(-fAngle) * (1.0f / fScale);
    float dvCol = (float)cos(-fAngle) * (1.0f / fScale);
    float duRow = dvCol;
    float dvRow = -duCol;
 
    float startingu = fSrcCX - (fDstCX * dvCol + fDstCY * duCol);
    float startingv = fSrcCY - (fDstCX * dvRow + fDstCY * duRow);
 
    float rowu = startingu;
    float rowv = startingv;
 
    for(int y = 0; y < dstH; y++)
    {
        float u = rowu;
        float v = rowv;
     
        WORD *pDst = pDstBase + (dstDelta * y);
 
        for(int x = 0; x < dstW ; x++)
        {   
            WORD *pSrc = pSrcBase + (((int)u) & srcW-1) + 
                         ((((int)v) & srcH-1) * srcDelta );
                         
            *pDst++ = *pSrc++;
 
            u += duRow;
            v += dvRow;
        }
 
        rowu += duCol;
        rowv += dvCol;
    }
}
 
//////////////////////////////////////////////////////////////////
// Rotate - nowrapping clipping version
//
// this version does not have the limitation of power of 2 
// dimensions and will clip the source image instead of wrapping.
//////////////////////////////////////////////////////////////////
 
void RotateWithClip(
    WORD *pDstBase, int dstW, int dstH, int dstDelta,
    WORD *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstCX, float fDstCY,
    float fSrcCX, float fSrcCY, 
    float fAngle, float fScale)
{   
    srcDelta /= sizeof(WORD);
    dstDelta /= sizeof(WORD);
 
    float duCol = (float)sin(-fAngle) * (1.0f / fScale);
    float dvCol = (float)cos(-fAngle) * (1.0f / fScale);
    float duRow = dvCol;
    float dvRow = -duCol;
 
    float startingu = fSrcCX - (fDstCX * dvCol + fDstCY * duCol);
    float startingv = fSrcCY - (fDstCX * dvRow + fDstCY * duRow);
 
    float rowu = startingu;
    float rowv = startingv;
 
 
    for(int y = 0; y < dstH; y++)
    {
        float u = rowu;
        float v = rowv;
     
        WORD *pDst = pDstBase + (dstDelta * y);
 
        for(int x = 0; x < dstW ; x++)
        {   
            if(u>0 && v>0 && u<srcW && v<srcH)
            {   
                WORD *pSrc = pSrcBase + (int)u + 
                                ((int)v * srcDelta);
                     
                *pDst++ = *pSrc++;
            }
            else
            {
                *pDst++ = 0;
            }
 
            u += duRow;
            v += dvRow;
        }
 
        rowu += duCol;
        rowv += dvCol;
    }
}
//////////////////////////////////////////////////////////////////
 
//End of File
