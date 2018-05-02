#ifndef ROTATE_H_INCLUDED
#define ROTATE_H_INCLUDED

#ifndef WDIBPIXEL_DEFINED
#define WDIBPIXEL_DEFINED
#include <stdint.h>
#if WDIBPIXEL_BITS==16
typedef uint16_t WDIBPIXEL;
//#define WDIBPIXEL_BITS (16) 
#else
typedef uint32_t WDIBPIXEL;
#define WDIBPIXEL_BITS (32) 
#endif
#endif

///////////////////////////////////////////////////////////////////
void RotateWrapFill(
    WDIBPIXEL *pDstBase, int dstW, int dstH, int dstDelta,
    WDIBPIXEL *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstRotCenterX, float fDstRotCenterY,
    float fSrcRotCenterX, float fSrcRotCenterY, 
    float fAngle, float fScale);
 
///////////////////////////////////////////////////////////////////
void RotateDrawWithClip(
    WDIBPIXEL *pDstBase, int dstW, int dstH, int dstDelta,
    WDIBPIXEL *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstRotCenterX, float fDstRotCenterY,
    float fSrcRotCenterX, float fSrcRotCenterY, 
    float fAngle, float fScale);

///////////////////////////////////////////////////////////////////
void RotateDrawWithClipAlt
    (
        WDIBPIXEL *dst, int dstW, int dstH, int dstDelta, 
        WDIBPIXEL *src, int srcW, int srcH, int srcDelta,
        double ox, double oy, 
        double px, double py, 
        double angle, double scale
    );

#endif
