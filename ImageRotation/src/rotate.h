#ifndef ROTATE_H_INCLUDED
#define ROTATE_H_INCLUDED

#ifndef ROTATEPIXEL_T_DEFINED
#define ROTATEPIXEL_T_DEFINED
#include <stdint.h>
#if ROTATEPIXEL_T_BITS==16
typedef uint16_t RotatePixel_t;
//#define ROTATEPIXEL_T_BITS (16) 
#else
typedef uint32_t RotatePixel_t;
#define ROTATEPIXEL_T_BITS (32) 
#endif
#endif

typedef RotatePixel_t (*RotateColorMergerFunc_t)(RotatePixel_t newColor, RotatePixel_t oldColor, void *Param);

#ifdef __cplusplus
#define ROTATE_DEF_PARAM(val) = val
#else
#define ROTATE_DEF_PARAM(val) // None
#endif

///////////////////////////////////////////////////////////////////
void RotateWrapFill(
    RotatePixel_t *pDstBase, int dstW, int dstH, int dstDelta,
    RotatePixel_t *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstRotCenterX, float fDstRotCenterY,
    float fSrcRotCenterX, float fSrcRotCenterY, 
    float fAngle, float fScale);
 
///////////////////////////////////////////////////////////////////
void RotateDrawWithClip(
    RotatePixel_t *pDstBase, int dstW, int dstH, int dstDelta,
    RotatePixel_t *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstRotCenterX, float fDstRotCenterY,
    float fSrcRotCenterX, float fSrcRotCenterY, 
    float fAngle, float fScale);

///////////////////////////////////////////////////////////////////
void RotateDrawWithClipAltD
    (
        RotatePixel_t *dst, int dstW, int dstH, int dstDelta, 
        RotatePixel_t *src, int srcW, int srcH, int srcDelta,
        double ox, double oy, 
        double px, double py, 
        double angle, double scale
    );

///////////////////////////////////////////////////////////////////
void RotateDrawWithClipAlt
    (
        RotatePixel_t *dst, int dstW, int dstH, int dstDelta, 
        RotatePixel_t *src, int srcW, int srcH, int srcDelta,
        float ox, float oy, 
        float px, float py, 
        float angle, float scale,
        RotateColorMergerFunc_t mergeFunc ROTATE_DEF_PARAM(NULL),
        void *mergeParam ROTATE_DEF_PARAM(NULL)
    );

///////////////////////////////////////////////////////////////////
void RotateDrawWithClipAlt2
    (
        RotatePixel_t *dst, int dstW, int dstH, int dstDelta, 
        RotatePixel_t *src, int srcW, int srcH, int srcDelta,
        float ox, float oy, 
        float px, float py, 
        float angle, float scale,
        RotateColorMergerFunc_t mergeFunc ROTATE_DEF_PARAM(NULL),
        void *mergeParam ROTATE_DEF_PARAM(NULL)
    );

#endif
