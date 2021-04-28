# ImageRotation
Fast Image Rotation algorithms

Playground for studing and implementing fast image rotation algoritms (with reasonable complexity).

The code here based on many [references](#References).

# Usage

These functions uses typed `RotatePixel_t` for single pixel type (by defaut: 32 bit). You may specify your own pixel type size `ROTATEPIXEL_T_BITS` (say, you may `#define ROTATEPIXEL_T_BITS 16` to use 16 bit wide pixles (values of 8/24/32 supported).

## RotateDrawClip

To rotate image and put it to the destination, use:
```
void RotateDrawClip
(
    RotatePixel_t* pDstBase, int dstW, int dstH, int dstDelta,
    RotatePixel_t* pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstRotCenterX, float fDstRotCenterY,
    float fSrcRotCenterX, float fSrcRotCenterY,
    float fAngle, float fScale
);
```

## RotateDrawFill

To rotate image and fill the whole destination, use:
```
void RotateDrawFill
(
    RotatePixel_t *pDstBase, int dstW, int dstH, int dstDelta,
    RotatePixel_t *pSrcBase, int srcW, int srcH, int srcDelta,
    float fDstRotCenterX, float fDstRotCenterY,
    float fSrcRotCenterX, float fSrcRotCenterY, 
    float fAngle, float fScale
);
```

# References

The References (and Thanks to!):

* "Fast Bitmap Rotation and Scaling" By Steven Mortimer, Dr Dobbs' Journal, July 01, 2001"<br/>
   http://www.drdobbs.com/architecture-and-design/fast-bitmap-rotation-and-scaling/184416337
* "Rotate Bitmap Any Angle Using Scanline Property" <br/>
   http://www.efg2.com/Lab/ImageProcessing/RotateScanline.htm
* "Bitmap API" Project <br/>
   https://github.com/wernsey/bitmap
