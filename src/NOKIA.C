#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdarg.h>

#define NK_SCREEN_W 84
#define NK_SCREEN_H 48

#define NK_MODE_TILEMAP 0 // Draw using tiles and sprites.
#define NK_MODE_BITMAP  1 // Draw using primitives and sprites.

#define NK_TILE_W 7
#define NK_TILE_H 8

#define NK_SCREEN_W_TILES (NK_SCREEN_W/NK_TILE_W)
#define NK_SCREEN_H_TILES (NK_SCREEN_H/NK_TILE_H)

#define NK_SCREEN_TILES (NK_SCREEN_W_TILES*NK_SCREEN_H_TILES)

#define NK_MAX_SPRITES 1024

#define NKAPI static

#define NK_COLOR_0 0XFFC7F0D8 // Light
#define NK_COLOR_1 0XFF43523D // Dark

#define NK_FALSE 0
#define NK_TRUE  1

#define NK_ASSERT(e)         assert(e)
#define NK_ZERO_MEM(    x)   memset(&(x), 0, sizeof( (x)))
#define NK_ZERO_MEM_PTR(x)   memset( (x), 0, sizeof(*(x)))
#define NK_ZERO_MEM_ARR(x)   memset( (x), 0, sizeof( (x)))
#define NK_ARRAY_SIZE(a)     (sizeof(a)/(sizeof((a)[0])))
#define NK_CLAMP(x,low,high) (((x)>(high))?(high):(((x)<(low))?(low):(x)))
#define NK_ABS(a)            (((a)<0)?-(a):(a))
#define NK_SWAP(x,y,t)       do { t tmp__ = x; x = y; y = tmp__; } while (0)
#define NK_MIN(a,b)          (((a)<(b))?(a):(b))
#define NK_MAX(a,b)          (((a)>(b))?(a):(b))
#define NK_FLOOR(f)          NK_CAST(S32,(f))
#define NK_CEIL(f)           NK_CAST(S32,(f)+1.0f)
#define NK_ROUND(f)          (((f) < 0.0f) ? NK_CAST(S32,(f)-0.5f) : NK_CAST(S32,(f)+0.5f))
#define NK_CAST(t,x)         ((t)(x))

typedef  uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef   int8_t  S8;
typedef  int16_t S16;
typedef  int32_t S32;
typedef  int64_t S64;
typedef    float F32;
typedef   double F64;
typedef   int8_t  B8;
typedef  int16_t B16;
typedef  int32_t B32;
typedef  int64_t B64;

#include "TILE.C"
#include "FONT.C"

typedef enum
{
    NK_KEY_Q,
    NK_KEY_W,
    NK_KEY_E,
    NK_KEY_A,
    NK_KEY_S,
    NK_KEY_D,
    NK_KEY_Z,
    NK_KEY_X,
    NK_KEY_C,
    NK_KEY_SPACE,
    NK_KEY_COMMA,
    NK_KEY_POINT,
    NK_KEY_TOTAL

} nkKEY;

typedef enum
{
    NK_SND_NONE,
    NK_SND_BLIP01,
    NK_SND_BLIP02,
    NK_SND_BLIP03,
    NK_SND_BLIP04,
    NK_SND_BLIP05,
    NK_SND_BLIP06,
    NK_SND_BLIP07,
    NK_SND_BLIP08,
    NK_SND_BLIP09,
    NK_SND_BLIP10,
    NK_SND_BLIP11,
    NK_SND_BLIP12,
    NK_SND_BLIP13,
    NK_SND_BLIP14,
    NK_SND_C5,
    NK_SND_CRUST,
    NK_SND_GOOD01,
    NK_SND_GOOD02,
    NK_SND_GOOD03,
    NK_SND_HIT01,
    NK_SND_HIT02,
    NK_SND_HIT03,
    NK_SND_HIT04,
    NK_SND_HIT05,
    NK_SND_HIT06,
    NK_SND_JINGLE,
    NK_SND_MELODY,
    NK_SND_NEGTIV01,
    NK_SND_NEGTIV02,
    NK_SND_ODD01,
    NK_SND_ODD02,
    NK_SND_ODD03,
    NK_SND_ODD04,
    NK_SND_RING,
    NK_SND_SNDTEST,
    NK_SND_UNREAL,
    NK_SND_TOTAL

} nkSND;

typedef struct
{
    S32 x;
    S32 y;
    U8  index;

} nkSPRITE;

typedef struct
{
    S32  width;
    S32  height;
    S32  bits;
    U32* pixels;

} nkSCREEN;

typedef struct
{
    nkSND       sound;
    nkSPRITE    spriteList[NK_MAX_SPRITES];
    U16         sprites;
    U8          tileMap[NK_SCREEN_TILES];
    U8          textMap[NK_SCREEN_TILES];
    U8          mode;
    U32         frame;
    // Internal
    nkSCREEN    screen;
    B8          prevKeyState[NK_KEY_TOTAL];
    B8          currKeyState[NK_KEY_TOTAL];
    S32         viewportX;
    S32         viewportY;
    S32         viewportW;
    S32         viewportH;

} nkCONTEXT;

NKAPI void nkPlaySound (nkCONTEXT* nokia, nkSND snd)
{
    NK_ASSERT(nokia && snd >= 0 && snd < NK_SND_TOTAL);
    nokia->sound = snd;
}

NKAPI void nkDrawSprite (nkCONTEXT* nokia, S32 x, S32 y, U8 index)
{
    NK_ASSERT(nokia && nokia->sprites < NK_MAX_SPRITES);
    nkSPRITE* spr = &nokia->spriteList[nokia->sprites++];
    spr->x = x, spr->y = y, spr->index = index;
}

NKAPI void nkSetText (nkCONTEXT* nokia, S32 tileX, S32 tileY, B8 invert, const char* text, ...)
{
    NK_ASSERT(nokia);

    if (tileX < 0 || tileX > NK_SCREEN_W_TILES-1) return;
    if (tileY < 0 || tileY > NK_SCREEN_H_TILES-1) return;

    const U32 TEXT_BUFFER_SIZE = 256;
    char textBuffer[TEXT_BUFFER_SIZE];
    NK_ZERO_MEM_ARR(textBuffer);

    // Format the arguments into a formatted string in a buffer.
    va_list args;
    va_start(args, text);
    vsnprintf(textBuffer, TEXT_BUFFER_SIZE, text, args);
    va_end(args);

    S32 startX = tileX;
    S32 startY = tileY;

    U8 offset = (invert) ? 0x80 : 0x00;

    for (const char* c=textBuffer; *c; ++c)
    {
        if (tileX >= 0 && tileX < NK_SCREEN_W_TILES &&
            tileY >= 0 && tileY < NK_SCREEN_H_TILES)
        {
            nokia->textMap[tileY*NK_SCREEN_W_TILES+tileX] = NK_CAST(U8,(*c))+offset;

            if (*c == '\n')
            {
                tileX = startX;
                tileY++;
            }
            else
            {
                tileX++;
            }
        }
    }
}

NKAPI void nkDrawClear (nkCONTEXT* nokia)
{
    for (S32 i=0, n=(NK_SCREEN_W*NK_SCREEN_H); i<n; ++i)
    {
        nokia->screen.pixels[i] = NK_COLOR_0;
    }
}

NKAPI void nkDrawPoint (nkCONTEXT* nokia, S32 x, S32 y)
{
    NK_ASSERT(nokia && nokia->mode == NK_MODE_BITMAP);
    if (x < 0 || x >= NK_SCREEN_W || y < 0 || y >= NK_SCREEN_H) return;
    nokia->screen.pixels[y*NK_SCREEN_W+x] = NK_COLOR_1;
}

NKAPI void nkDrawLine (nkCONTEXT* nokia, S32 x1, S32 y1, S32 x2, S32 y2)
{
    NK_ASSERT(nokia && nokia->mode == NK_MODE_BITMAP);

    if (x1 > NK_SCREEN_W && x2 > NK_SCREEN_W) return;
    if (x1 < 0 && x2 < 0) return;
    if (y1 > NK_SCREEN_H && y2 > NK_SCREEN_H) return;
    if (y1 < 0 && y2 < 0) return;

    // Clamp the bounds to avoid overflows.
    x1 = NK_CLAMP(x1, 0, NK_SCREEN_W);
    y1 = NK_CLAMP(y1, 0, NK_SCREEN_H);
    x2 = NK_CLAMP(x2, 0, NK_SCREEN_W);
    y2 = NK_CLAMP(y2, 0, NK_SCREEN_H);

    B8 steep = NK_FALSE;
    if (NK_ABS(x1-x2)<NK_ABS(y1-y2))
    {
        NK_SWAP(x1, y1, S32);
        NK_SWAP(x2, y2, S32);
        steep = NK_TRUE;
    }
    if (x1>x2)
    {
        NK_SWAP(x1, x2, S32);
        NK_SWAP(y1, y2, S32);
    }
    S32 dx = x2-x1;
    S32 dy = y2-y1;
    S32 derror2 = NK_ABS(dy)*2;
    S32 error2 = 0;
    S32 iy = y1;

    for (S32 ix=x1; ix<=x2; ++ix)
    {
        if (steep) nokia->screen.pixels[ix*NK_SCREEN_W+iy] = NK_COLOR_1;
        else nokia->screen.pixels[iy*NK_SCREEN_W+ix] = NK_COLOR_1;
        error2 += derror2;
        if (error2 > dx)
        {
            iy += (y2>y1?1:-1);
            error2 -= dx*2;
        }
    }
}

NKAPI void nkDrawRect (nkCONTEXT* nokia, S32 x, S32 y, S32 w, S32 h)
{
    NK_ASSERT(nokia && nokia->mode == NK_MODE_BITMAP);

    if (x > NK_SCREEN_W) return;
    if (y > NK_SCREEN_H) return;

    S32 x1 = x;
    S32 y1 = y;
    S32 x2 = x+w-1;
    S32 y2 = y+h-1;

    nkDrawLine(nokia, x2,y1,x2,y2); // Right
    nkDrawLine(nokia, x1,y1,x1,y2); // Left
    nkDrawLine(nokia, x1,y1,x2,y1); // Top
    nkDrawLine(nokia, x1,y2,x2,y2); // Bottom
}

NKAPI void nkDrawFill (nkCONTEXT* nokia, S32 x, S32 y, S32 w, S32 h)
{
    NK_ASSERT(nokia && nokia->mode == NK_MODE_BITMAP);

    if (x > NK_SCREEN_W-1) return;
    if (y > NK_SCREEN_H-1) return;

    S32 x1 = x;
    S32 y1 = y;
    S32 x2 = x+w-1;
    S32 y2 = y+h-1;

    // Clamp the bounds to avoid overflows.
    x1 = NK_CLAMP(x1, 0, NK_SCREEN_W);
    y1 = NK_CLAMP(y1, 0, NK_SCREEN_H);
    x2 = NK_CLAMP(x2, 0, NK_SCREEN_W);
    y2 = NK_CLAMP(y2, 0, NK_SCREEN_H);

    for (S32 iy=y1; iy<=y2; ++iy)
    {
        for (S32 ix=x1; ix<=x2; ++ix)
        {
            nokia->screen.pixels[iy*NK_SCREEN_W+ix] = NK_COLOR_1;
        }
    }
}

NKAPI void nkDrawCircle (nkCONTEXT* nokia, S32 x, S32 y, S32 r, S32 thickness)
{
    NK_ASSERT(nokia && nokia->mode == NK_MODE_BITMAP);

    S32 outer = r;
    S32 inner = outer-thickness+1;

    S32 xo   = outer;
    S32 xi   = inner;
    S32 yy   = 0;
    S32 erro = 1-xo;
    S32 erri = 1-xi;

    while (xo >= yy)
    {
        nkDrawLine(nokia, x+xi, y+yy, x+xo, y+yy);
        nkDrawLine(nokia, x+yy, y+xi, x+yy, y+xo);
        nkDrawLine(nokia, x-xo, y+yy, x-xi, y+yy);
        nkDrawLine(nokia, x-yy, y+xi, x-yy, y+xo);
        nkDrawLine(nokia, x-xo, y-yy, x-xi, y-yy);
        nkDrawLine(nokia, x-yy, y-xo, x-yy, y-xi);
        nkDrawLine(nokia, x+xi, y-yy, x+xo, y-yy);
        nkDrawLine(nokia, x+yy, y-xo, x+yy, y-xi);

        yy++;

        if (erro < 0)
        {
            erro += 2*yy+1;
        }
        else
        {
            xo--;
            erro += 2*(yy-xo+1);
        }

        if (yy > inner)
        {
            xi = yy;
        }
        else
        {
            if (erri < 0)
            {
                erri += 2*yy+1;
            }
            else
            {
                xi--;
                erri += 2*(yy-xi+1);
            }
        }
    }
}

NKAPI B8 nkKeyPressed (nkCONTEXT* nokia, nkKEY key)
{
    NK_ASSERT(nokia && key >= 0 && key < NK_KEY_TOTAL);
    return (!nokia->prevKeyState[key] && nokia->currKeyState[key]);
}
NKAPI B8 nkKeyReleased (nkCONTEXT* nokia, nkKEY key)
{
    NK_ASSERT(nokia && key >= 0 && key < NK_KEY_TOTAL);
    return (nokia->prevKeyState[key] && !nokia->currKeyState[key]);
}
NKAPI B8 nkKeyUp (nkCONTEXT* nokia, nkKEY key)
{
    NK_ASSERT(nokia && key >= 0 && key < NK_KEY_TOTAL);
    return (!nokia->currKeyState[key]);
}
NKAPI B8 nkKeyDown (nkCONTEXT* nokia, nkKEY key)
{
    NK_ASSERT(nokia && key >= 0 && key < NK_KEY_TOTAL);
    return (nokia->currKeyState[key]);
}

NKAPI void nkSeedRandom ()
{
    srand(NK_CAST(U32, time(NULL)));
}
NKAPI S32 nkRandomS32 ()
{
    return rand();
}
NKAPI S32 nkRandomRangeS32 (S32 min, S32 max)
{
    return ((rand() % ((max+1)-min)) + min);
}
NKAPI F32 nkRandomF32 ()
{
    return (NK_CAST(F32,rand()) / NK_CAST(F32,RAND_MAX));
}
NKAPI F32 nkRandomRangeF32 (F32 min, F32 max)
{
    return (min + NK_CAST(F32,rand()) / NK_CAST(F32,RAND_MAX/(max-min)));
}

NKAPI F32 nkSinRange (F32 min, F32 max, F32 t)
{
    F32 half = (max - min) / 2;
    return min + half + sinf(t) * half;
}

// Do not call these functions! It is used internally to convert the internal tile, text,
// and sprite data into a screen of pixels that can then be drawn to the user's display.
NKAPI void nkBeginFrame (nkCONTEXT* nokia)
{
    NK_ASSERT(nokia);
    nokia->sprites = 0;
    nkDrawClear(nokia);
}

NKAPI void nkEndFrame (nkCONTEXT* nokia)
{
    NK_ASSERT(nokia);

    nokia->frame++;

    // Tile and text maps will only draw in NK_MODE_TILEMAP.
    if (nokia->mode == NK_MODE_TILEMAP)
    {
        for (S32 iy=0; iy<NK_SCREEN_H_TILES; ++iy)
        {
            for (S32 ix=0; ix<NK_SCREEN_W_TILES; ++ix)
            {
                if (nokia->textMap[iy*NK_SCREEN_W_TILES+ix])
                {
                    for (S32 i=0; i<NK_TILE_H; ++i)
                    {
                        U8 row = NK_FONT[(nokia->textMap[iy*NK_SCREEN_W_TILES+ix]*NK_TILE_H)+i];
                        for (S32 j=0; j<NK_TILE_W; ++j)
                        {
                            if (row & (1 << (7-j)))
                            {
                                nokia->screen.pixels[((iy*NK_TILE_H)+i)*NK_SCREEN_W+((ix*NK_TILE_W)+j)] = NK_COLOR_1;
                            }
                        }
                    }
                }
                else
                {
                    if (nokia->tileMap[iy*NK_SCREEN_W_TILES+ix])
                    {
                        for (S32 i=0; i<NK_TILE_H; ++i)
                        {
                            U8 row = NK_TILE[(nokia->tileMap[iy*NK_SCREEN_W_TILES+ix]*NK_TILE_H)+i];
                            for (S32 j=0; j<NK_TILE_W; ++j)
                            {
                                if (row & (1 << (7-j)))
                                {
                                    nokia->screen.pixels[((iy*NK_TILE_H)+i)*NK_SCREEN_W+((ix*NK_TILE_W)+j)] = NK_COLOR_1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Sprites draw whether NK_MODE_TILEMAP or NK_MODE_BITMAP.
    for (S32 i=0; i<nokia->sprites; ++i)
    {
        nkSPRITE* spr = &nokia->spriteList[i];

        for (S32 i=0; i<NK_TILE_H; ++i)
        {
            U8 row = NK_TILE[(spr->index*NK_TILE_H)+i];
            for (S32 j=0; j<NK_TILE_W; ++j)
            {
                if (row & (1 << (7-j)))
                {
                    S32 x = (spr->x+j);
                    S32 y = (spr->y+i);

                    if (x >= 0 && x < NK_SCREEN_W && y >= 0 && y < NK_SCREEN_H)
                    {
                        nokia->screen.pixels[y*NK_SCREEN_W+x] = NK_COLOR_1;
                    }
                }
            }
        }
    }
}
