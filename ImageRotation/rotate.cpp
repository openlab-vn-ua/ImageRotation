#include "rotate.h"
#include <math.h>
 
#define DEBUG_DRAW 0
#define DEBUG_MARK_COLOR ((WDIBPIXEL)(0xFFFFFF))

/// <summary>
/// Checks if source value is power of 2
/// </summary>
static bool IsExp2(unsigned int value)
{
    return (value > 0 && (value & (value - 1)) == 0);
}

static // Foreward declaraion of faster func when srcW & srcH is power of 2
void RotateWrapFillFastSrcSizeExp2(
    WDIBPIXEL *pDstBase, int dstW, int dstH, int dstDelta,
    WDIBPIXEL *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstCX, float fDstCY,
    float fSrcCX, float fSrcCY,
    float fAngle, float fScale);

/// <summary>
/// Rotates source image and writes it to the destination, filling all the target.
/// This version takes any dimension source bitmap and wraps.
/// fAngle > 0 = (CW:for bottom-up bmp, CCW for top-bottom bmp)
/// </summary>
void RotateWrapFill(
    WDIBPIXEL *pDstBase, int dstW, int dstH, int dstDelta,
    WDIBPIXEL *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstCX, float fDstCY,
    float fSrcCX, float fSrcCY, 
    float fAngle, float fScale)
{
    if (IsExp2(srcW) && IsExp2(srcH))
    {
        RotateWrapFillFastSrcSizeExp2
        (
            pDstBase, dstW,dstH,dstDelta,
            pSrcBase,srcW, srcH, srcDelta,
            fDstCX, fDstCY,
            fSrcCX, fSrcCY,
            fAngle, fScale
        );
        return;
    }

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

            int sx = (int)u;
            int sy = (int)v;

            // Negative u/v adjustement
            // We need some additional proccessing for negative u and v values
            // value in range (-0.99..;0) should be mapped to last source pixel, not zero
            // Because zero source pixel already drawn at [0;0.99..)
            // (else we will observe double line of same colored pixels in when u/v flips from + to -)
            // Example: without shift u = -0.25 becimes sx=0, we need sx=-1, since we already had sx=0 at u=+0.25

            if (u < 0)
            {
                // Negative u/v adjustement
                sx--;
                sx = -sx % srcW; sx = srcW - sx;
            }

            sx %= srcW;

            if (v < 0)
            {
                // Negative u/v adjustement
                sy--;
                sy = -sy % srcH; sy = srcH - sy; 
            }

            sy %= srcH;

            WDIBPIXEL *pSrc = pSrcBase + sx + (sy * srcDelta);
                         
            *pDst++ = *pSrc++;

            u += duRow;
            v += dvRow;
        }

        rowu += duCol;
        rowv += dvCol;
    }
}

/// <summary>
/// Rotates source image and writes it to the destination, filling all the target.
/// This version takes any dimension source bitmap and wraps.
/// fAngle > 0 = (CW:for bottom-up bmp, CCW for top-bottom bmp)
/// IMPORTANT: This version assumes the dimensions of the source image to be a power of two.
/// </summary>
static
void RotateWrapFillFastSrcSizeExp2(
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

            int sx = (int)u;
            int sy = (int)v;

            // Negative u/v adjustement

            if (u < 0) { sx--; }
            if (v < 0) { sy--; }

            sx &= (srcW-1);
            sy &= (srcH-1);

            WDIBPIXEL *pSrc = pSrcBase + sx + (sy * srcDelta);

            *pDst++ = *pSrc++;

            u += duRow;
            v += dvRow;
        }

        rowu += duCol;
        rowv += dvCol;
    }
}

//#define VOID_COLOR 0

/// <summary>
/// Rotates source image and writes it to the destination.
/// Nowrapping clipping version.
/// Uncovered parts of target is kept as it was
/// fAngle > 0 = (CW:for bottom-up bmp, CCW for top-bottom bmp)
/// </summary>
void RotateDrawWithClip(
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

            int sx = (int)u;
            int sy = (int)v;

            // For non-negative values we have to check u and v (not sx and sy)
            // since u = -0.25 gives sx=0 after rounsing, so 1 extra pixel line will be drawn
            // (we assume that u,v >= 0 will lead to sx,sy >= 0)

            if ((u >= 0) && (v >= 0) && (sx < srcW) && (sy < srcH))
            {
                WDIBPIXEL *pSrc = pSrcBase + sx + (sy * srcDelta);

                *pDst++ = *pSrc++;
            }
            else
            {
                pDst++; // Skip
                //*pDst++ = VOID_COLOR; // Fill void (black)
            }

            u += duRow;
            v += dvRow;
        }

        rowu += duCol;
        rowv += dvCol;
    }
}
