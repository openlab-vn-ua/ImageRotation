#include "rotate.h"
#include <math.h>
 
#define DEBUG_DRAW 1
#define DEBUG_MARK_COLOR ((WDIBPIXEL)(0xFFFFFF))

//////////////////////////////////////////////////////////////////
// Rotate - wrapping version
// This version takes any dimension source bitmap and wraps.
//////////////////////////////////////////////////////////////////
void Rotate(
    WDIBPIXEL *pDstBase, int dstW, int dstH, int dstDelta,
    WDIBPIXEL *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstCX, float fDstCY,
    float fSrcCX, float fSrcCY, 
    float fAngle, float fScale)
{   
    if (dstW <= 0) { return; }
    if (dstH <= 0) { return; }
 
    srcDelta /= sizeof(WDIBPIXEL);
    dstDelta /= sizeof(WDIBPIXEL);
 
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
     
        WDIBPIXEL *pDst = pDstBase + (dstDelta * y);
 
        for(int x = 0; x < dstW ; x++)
        {   
            #if DEBUG_DRAW
            if ((int(u) == int(fSrcCX)) && (int(v) == int(fSrcCY)))
            {
                *pDst++ = DEBUG_MARK_COLOR;
                u += duRow;
                v += dvRow;
                continue;
            }
            #endif

            int sx = ((u < 0.0 ? (int)-u : (int)u) + srcW) % srcW;
            int sy = ((v < 0.0 ? (int)-v : (int)v) + srcH) % srcH;
 
            WDIBPIXEL *pSrc = pSrcBase + sx + (sy * srcDelta);
                         
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
    WDIBPIXEL *pDstBase, int dstW, int dstH, int dstDelta,
    WDIBPIXEL *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstCX, float fDstCY,
    float fSrcCX, float fSrcCY, 
    float fAngle, float fScale)
{   
    if (dstW <= 0) { return; }
    if (dstH <= 0) { return; }

    srcDelta /= sizeof(WDIBPIXEL);
    dstDelta /= sizeof(WDIBPIXEL);
 
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
     
        WDIBPIXEL *pDst = pDstBase + (dstDelta * y);
 
        for(int x = 0; x < dstW ; x++)
        {   
            #if DEBUG_DRAW
            if ((int(u) == int(fSrcCX)) && (int(v) == int(fSrcCY)))
            {
                *pDst++ = DEBUG_MARK_COLOR;
                u += duRow;
                v += dvRow;
                continue;
            }
            #endif

            WDIBPIXEL *pSrc = pSrcBase + (((int)u) & srcW-1) + 
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
    WDIBPIXEL *pDstBase, int dstW, int dstH, int dstDelta,
    WDIBPIXEL *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstCX, float fDstCY,
    float fSrcCX, float fSrcCY, 
    float fAngle, float fScale)
{   
    if (dstW <= 0) { return; }
    if (dstH <= 0) { return; }

    srcDelta /= sizeof(WDIBPIXEL);
    dstDelta /= sizeof(WDIBPIXEL);
 
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
     
        WDIBPIXEL *pDst = pDstBase + (dstDelta * y);
 
        for(int x = 0; x < dstW ; x++)
        {   
            #if DEBUG_DRAW
            if ((int(u) == int(fSrcCX)) && (int(v) == int(fSrcCY)))
            {
                *pDst++ = DEBUG_MARK_COLOR;
                u += duRow;
                v += dvRow;
                continue;
            }
            #endif

            if(u>0 && v>0 && u<srcW && v<srcH)
            {   
                WDIBPIXEL *pSrc = pSrcBase + (int)u + 
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
