#ifndef ROTATE_H_INCLUDED
#define ROTATE_H_INCLUDED

#include <stdlib.h>

#ifndef ROTATEPIXEL_T_DEFINED
#define ROTATEPIXEL_T_DEFINED
#include <stdint.h>
#if ROTATEPIXEL_T_BITS==8
typedef char RotatePixel_t; // Pixel type for rotation library
//#define ROTATEPIXEL_T_BITS (8)
#elif ROTATEPIXEL_T_BITS==16
typedef uint16_t RotatePixel_t; // Pixel type for rotation library
//#define ROTATEPIXEL_T_BITS (16)
#elif ROTATEPIXEL_T_BITS==24
typedef struct { char _[3]; } RotatePixel_t; // Pixel type for rotation library
//#define ROTATEPIXEL_T_BITS (24)
#else // 32
typedef uint32_t RotatePixel_t; // Pixel type for rotation library
#define ROTATEPIXEL_T_BITS (32)
#endif
#endif

/// <summary> Merge pixel function callback to merge old and new color </summary>
typedef RotatePixel_t (*RotateColorMergerFunc_t)(RotatePixel_t newColor, RotatePixel_t oldColor, void *Param);

#ifdef __cplusplus
#define ROTATE_DEF_PARAM(val) = val
#else
#define ROTATE_DEF_PARAM(val) // None
#endif

/// <summary>
/// Rotates source image and writes it to the destination, filling all the target.
/// All destination filled by rotated and scaled copies of source tiles.
/// </summary>
/// <param name="pDstBase">Destination image data address</param>
/// <param name="dstW">Destination image width in pixels</param>
/// <param name="dstH">Destination image height in pixels</param>
/// <param name="dstDelta">Destination image horizontal scan line size in bytes (AKA stride)</param>
/// <param name="pSrcBase">Source image data address</param>
/// <param name="srcW">Source image width in pixels</param>
/// <param name="srcH">Source image height in pixels</param>
/// <param name="srcDelta">Source image horizontal scan line size </param>
/// <param name="fDstRotCenterX">Rotation center X location in destination image</param>
/// <param name="fDstRotCenterY">Rotation center Y location in destination image</param>
/// <param name="fSrcRotCenterX">Rotation center X location in source image</param>
/// <param name="fSrcRotCenterY">Rotation center Y location in source image</param>
/// <param name="fAngle">Angle of rotation in radians. (if Angle > 0 : CCW for top-bottom [normal] bmp, CW:for bottom-up [reverced] bmp)</param>
/// <param name="fScale">Scale of source before apply to destination. (Scale = 1 : no scale)</param>
extern 
void RotateDrawFill
(
    RotatePixel_t *pDstBase, int dstW, int dstH, int dstDelta,
    RotatePixel_t *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstRotCenterX, float fDstRotCenterY,
    float fSrcRotCenterX, float fSrcRotCenterY, 
    float fAngle, float fScale
);
 
/// <summary>
/// Rotate source image and put it on destination image.
/// One copy of rotated and scaled source drawn on target
/// [Default version] [Now via RotateDrawClipExt2]
/// </summary>
/// <param name="pDstBase">Destination image data address</param>
/// <param name="dstW">Destination image width in pixels</param>
/// <param name="dstH">Destination image height in pixels</param>
/// <param name="dstDelta">Destination image horizontal scan line size in bytes (AKA stride)</param>
/// <param name="pSrcBase">Source image data address</param>
/// <param name="srcW">Source image width in pixels</param>
/// <param name="srcH">Source image height in pixels</param>
/// <param name="srcDelta">Source image horizontal scan line size </param>
/// <param name="fDstRotCenterX">Rotation center X location in destination image</param>
/// <param name="fDstRotCenterY">Rotation center Y location in destination image</param>
/// <param name="fSrcRotCenterX">Rotation center X location in source image</param>
/// <param name="fSrcRotCenterY">Rotation center Y location in source image</param>
/// <param name="fAngle">Angle of rotation in radians. (if Angle > 0 : CCW for top-bottom [normal] bmp, CW:for bottom-up [reverced] bmp)</param>
/// <param name="fScale">Scale of source before apply to destination. (Scale = 1 : no scale)</param>
extern
void RotateDrawClip
(
    RotatePixel_t* pDstBase, int dstW, int dstH, int dstDelta,
    RotatePixel_t* pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstRotCenterX, float fDstRotCenterY,
    float fSrcRotCenterX, float fSrcRotCenterY,
    float fAngle, float fScale
);

/// <summary>
/// Rotate source image and put it on destination image.
/// One copy of rotated and scaled source drawn on target
/// Optional external function may be provided for color merge
/// [Default version] [Now via RotateDrawClipExt2]
/// </summary>
/// <param name="pDstBase">Destination image data address</param>
/// <param name="dstW">Destination image width in pixels</param>
/// <param name="dstH">Destination image height in pixels</param>
/// <param name="dstDelta">Destination image horizontal scan line size in bytes (AKA stride)</param>
/// <param name="pSrcBase">Source image data address</param>
/// <param name="srcW">Source image width in pixels</param>
/// <param name="srcH">Source image height in pixels</param>
/// <param name="srcDelta">Source image horizontal scan line size </param>
/// <param name="fDstRotCenterX">Rotation center X location in destination image</param>
/// <param name="fDstRotCenterY">Rotation center Y location in destination image</param>
/// <param name="fSrcRotCenterX">Rotation center X location in source image</param>
/// <param name="fSrcRotCenterY">Rotation center Y location in source image</param>
/// <param name="fAngle">Angle of rotation in radians. (if Angle > 0 : CCW for top-bottom [normal] bmp, CW:for bottom-up [reverced] bmp)</param>
/// <param name="fScale">Scale of source before apply to destination. (Scale = 1 : no scale)</param>
/// <param name="mergeFunc">Callback function to merge new and old pixel value [RotatePixel_t Merger(RotatePixel_t newColor, RotatePixel_t oldColor, void *Param)] NULL=not used</param>
/// <param name="mergeParam">Callback function last parameter NULL=not used</param>
extern
void RotateDrawClipExt
(
    RotatePixel_t* pDstBase, int dstW, int dstH, int dstDelta,
    RotatePixel_t* pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstRotCenterX, float fDstRotCenterY,
    float fSrcRotCenterX, float fSrcRotCenterY,
    float fAngle, float fScale,
    RotateColorMergerFunc_t mergeFunc ROTATE_DEF_PARAM(NULL),
    void* mergeParam ROTATE_DEF_PARAM(NULL)
);

// Individual versions
// --------------------------------------------------------
// (different implemenation alogorithms) -- for test only

/// <summary>
/// Rotate source image and put it on destination image.
/// One copy of rotated and scaled source drawn on target
/// [Standard prec version]
/// </summary>
/// <param name="pDstBase">Destination image data address</param>
/// <param name="dstW">Destination image width in pixels</param>
/// <param name="dstH">Destination image height in pixels</param>
/// <param name="dstDelta">Destination image horizontal scan line size in bytes (AKA stride)</param>
/// <param name="pSrcBase">Source image data address</param>
/// <param name="srcW">Source image width in pixels</param>
/// <param name="srcH">Source image height in pixels</param>
/// <param name="srcDelta">Source image horizontal scan line size </param>
/// <param name="fDstRotCenterX">Rotation center X location in destination image</param>
/// <param name="fDstRotCenterY">Rotation center Y location in destination image</param>
/// <param name="fSrcRotCenterX">Rotation center X location in source image</param>
/// <param name="fSrcRotCenterY">Rotation center Y location in source image</param>
/// <param name="fAngle">Angle of rotation in radians. (if Angle > 0 : CCW for top-bottom [normal] bmp, CW:for bottom-up [reverced] bmp)</param>
/// <param name="fScale">Scale of source before apply to destination. (Scale = 1 : no scale)</param>
extern
void RotateDrawClip1
(
    RotatePixel_t *pDstBase, int dstW, int dstH, int dstDelta,
    RotatePixel_t *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstRotCenterX, float fDstRotCenterY,
    float fSrcRotCenterX, float fSrcRotCenterY, 
    float fAngle, float fScale
);

/// <summary>
/// Rotate source image and put it on destination image.
/// One copy of rotated and scaled source drawn on target.
/// Optional external function may be provided for color merge
/// [Fast version]
/// </summary>
/// <param name="dst">Destination image data address</param>
/// <param name="dstW">Destination image width in pixels</param>
/// <param name="dstH">Destination image height in pixels</param>
/// <param name="dstDelta">Destination image horizontal scan line size in bytes (AKA stride)</param>
/// <param name="src">Source image data address</param>
/// <param name="srcW">Source image width in pixels</param>
/// <param name="srcH">Source image height in pixels</param>
/// <param name="srcDelta">Source image horizontal scan line size </param>
/// <param name="ox">Rotation center X location in destination image</param>
/// <param name="oy">Rotation center Y location in destination image</param>
/// <param name="px">Rotation center X location in source image</param>
/// <param name="py">Rotation center Y location in source image</param>
/// <param name="angle">Angle of rotation in radians. (if Angle > 0 : CCW for top-bottom [normal] bmp, CW:for bottom-up [reverced] bmp)</param>
/// <param name="scale">Scale of source before apply to destination. (Scale = 1 : no scale)</param>
/// <param name="mergeFunc">Callback function to merge new and old pixel value [RotatePixel_t Merger(RotatePixel_t newColor, RotatePixel_t oldColor, void *Param)] NULL=not used</param>
/// <param name="mergeParam">Callback function last parameter NULL=not used</param>
extern
void RotateDrawClipExt1
(
    RotatePixel_t *dst, int dstW, int dstH, int dstDelta, 
    RotatePixel_t *src, int srcW, int srcH, int srcDelta,
    float ox, float oy, 
    float px, float py, 
    float angle, float scale,
    RotateColorMergerFunc_t mergeFunc ROTATE_DEF_PARAM(NULL),
    void *mergeParam ROTATE_DEF_PARAM(NULL)
);

/// <summary>
/// Rotate source image and put it on destination image.
/// One copy of rotated and scaled source drawn on target.
/// Optional external function may be provided for color merge
/// [Fast version] [Double prec arguments]
/// </summary>
/// <param name="dst">Destination image data address</param>
/// <param name="dstW">Destination image width in pixels</param>
/// <param name="dstH">Destination image height in pixels</param>
/// <param name="dstDelta">Destination image horizontal scan line size in bytes (AKA stride)</param>
/// <param name="src">Source image data address</param>
/// <param name="srcW">Source image width in pixels</param>
/// <param name="srcH">Source image height in pixels</param>
/// <param name="srcDelta">Source image horizontal scan line size </param>
/// <param name="ox">Rotation center X location in destination image</param>
/// <param name="oy">Rotation center Y location in destination image</param>
/// <param name="px">Rotation center X location in source image</param>
/// <param name="py">Rotation center Y location in source image</param>
/// <param name="angle">Angle of rotation in radians. (if Angle > 0 : CCW for top-bottom [normal] bmp, CW:for bottom-up [reverced] bmp)</param>
/// <param name="scale">Scale of source before apply to destination. (Scale = 1 : no scale)</param>
/// <param name="mergeFunc">Callback function to merge new and old pixel value [RotatePixel_t Merger(RotatePixel_t newColor, RotatePixel_t oldColor, void *Param)] NULL=not used</param>
/// <param name="mergeParam">Callback function last parameter NULL=not used</param>
extern
void RotateDrawClipExt1D
(
    RotatePixel_t* dst, int dstW, int dstH, int dstDelta,
    RotatePixel_t* src, int srcW, int srcH, int srcDelta,
    double ox, double oy,
    double px, double py,
    double angle, double scale,
    RotateColorMergerFunc_t mergeFunc ROTATE_DEF_PARAM(NULL),
    void* mergeParam ROTATE_DEF_PARAM(NULL)
);

/// <summary>
/// Rotate source image and put it on destination image.
/// One copy of rotated and scaled source drawn on target.
/// Optional external function may be provided for color merge
/// [Extra Fast version] [Uses fixed point integers inside]
/// </summary>
/// <param name="dst">Destination image data address</param>
/// <param name="dstW">Destination image width in pixels</param>
/// <param name="dstH">Destination image height in pixels</param>
/// <param name="dstDelta">Destination image horizontal scan line size in bytes (AKA stride)</param>
/// <param name="src">Source image data address</param>
/// <param name="srcW">Source image width in pixels</param>
/// <param name="srcH">Source image height in pixels</param>
/// <param name="srcDelta">Source image horizontal scan line size </param>
/// <param name="ox">Rotation center X location in destination image</param>
/// <param name="oy">Rotation center Y location in destination image</param>
/// <param name="px">Rotation center X location in source image</param>
/// <param name="py">Rotation center Y location in source image</param>
/// <param name="angle">Angle of rotation in radians. (if Angle > 0 : CCW for top-bottom [normal] bmp, CW:for bottom-up [reverced] bmp)</param>
/// <param name="scale">Scale of source before apply to destination. (Scale = 1 : no scale)</param>
/// <param name="mergeFunc">Callback function to merge new and old pixel value [RotatePixel_t Merger(RotatePixel_t newColor, RotatePixel_t oldColor, void *Param)] NULL=not used</param>
/// <param name="mergeParam">Callback function last parameter NULL=not used</param>
extern
void RotateDrawClipExt2
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
