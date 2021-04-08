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

typedef WDIBPIXEL (*RotateColorMergerFunc_t)(WDIBPIXEL newColor, WDIBPIXEL oldColor, void *Param);

#ifdef __cplusplus
#define ROTATE_DEF_PARAM(val) = val
#else
#define ROTATE_DEF_PARAM(val) // None
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
void RotateDrawWithClipAltD
    (
        WDIBPIXEL *dst, int dstW, int dstH, int dstDelta, 
        WDIBPIXEL *src, int srcW, int srcH, int srcDelta,
        double ox, double oy, 
        double px, double py, 
        double angle, double scale
    );

///////////////////////////////////////////////////////////////////
void RotateDrawWithClipAlt
    (
        WDIBPIXEL *dst, int dstW, int dstH, int dstDelta, 
        WDIBPIXEL *src, int srcW, int srcH, int srcDelta,
        float ox, float oy, 
        float px, float py, 
        float angle, float scale,
        RotateColorMergerFunc_t mergeFunc ROTATE_DEF_PARAM(NULL),
        void *mergeParam ROTATE_DEF_PARAM(NULL)
    );

///////////////////////////////////////////////////////////////////
void RotateDrawWithClipAlt2
    (
        WDIBPIXEL *dst, int dstW, int dstH, int dstDelta, 
        WDIBPIXEL *src, int srcW, int srcH, int srcDelta,
        float ox, float oy, 
        float px, float py, 
        float angle, float scale,
        RotateColorMergerFunc_t mergeFunc ROTATE_DEF_PARAM(NULL),
        void *mergeParam ROTATE_DEF_PARAM(NULL)
    );

#endif
