#ifndef ROTATE_H_INCLUDED
#define ROTATE_H_INCLUDED

#if !defined(WDIBPIXEL_BITS)
#include <stdint.h>
typedef uint16_t WDIBPIXEL;
#define WDIBPIXEL_BITS (sizeof(WDIBPIXEL)*8)
#endif

///////////////////////////////////////////////////////////////////
void Rotate(
    WDIBPIXEL *pDstBase, int dstW, int dstH, int dstDelta,
    WDIBPIXEL *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstRotCenterX, float fDstRotCenterY,
    float fSrcRotCenterX, float fSrcRotCenterY, 
    float fAngle, float fScale);
 
///////////////////////////////////////////////////////////////////
void FastRotate(
    WDIBPIXEL *pDstBase, int dstW, int dstH, int dstDelta,
    WDIBPIXEL *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstRotCenterX, float fDstRotCenterY,
    float fSrcRotCenterX, float fSrcRotCenterY, 
    float fAngle, float fScale);
 
///////////////////////////////////////////////////////////////////
void RotateWithClip(
    WDIBPIXEL *pDstBase, int dstW, int dstH, int dstDelta,
    WDIBPIXEL *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstRotCenterX, float fDstRotCenterY,
    float fSrcRotCenterX, float fSrcRotCenterY, 
    float fAngle, float fScale);

#endif
