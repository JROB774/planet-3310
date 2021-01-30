#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#define NK_SCREEN_W 84
#define NK_SCREEN_H 48

#define NK_TILE_W 7
#define NK_TILE_H 8

#define NK_SCREEN_W_TILES (NK_SCREEN_W/NK_TILE_W)
#define NK_SCREEN_H_TILES (NK_SCREEN_H/NK_TILE_H)

#define NK_SCREEN_TILES (NK_SCREEN_W_TILES*NK_SCREEN_H_TILES)

#define NK_MAX_SPRITES 16

#define NKAPI static

#define NK_COLOR_0 0XFFC7F0D8 // Light
#define NK_COLOR_1 0XFF43523D // Dark

#define NK_FALSE 0
#define NK_TRUE  1

#define NK_STRUCT(name) typedef struct name##_T name; struct name##_T
#define NK_ENUM(name)   typedef enum   name##_T name; enum   name##_T

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

#include "TILESET.C"
#include "FONT.C"

NK_ENUM(nkKEY)
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
};

NK_ENUM(nkSND)
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
    NK_SND_OOD02,
    NK_SND_ODD03,
    NK_SND_ODD04,
    NK_SND_RING,
    NK_SND_SNDTEST,
    NK_SND_UNREAL,
    NK_SND_TOTAL
};

NK_STRUCT(nkSPRITE)
{
    S32 x;
    S32 y;
    U8  index;
};

NK_STRUCT(nkSCREEN)
{
    S32  width;
    S32  height;
    S32  bits;
    U32* pixels;
};

NK_STRUCT(nkCONTEXT)
{
    const char* sound;
    nkSPRITE    spriteList[NK_MAX_SPRITES];
    U8          tileMap[NK_SCREEN_TILES];
    U8          textMap[NK_SCREEN_TILES];
    // Internal
    nkSCREEN    screen;
    B8          prevKeyState[NK_KEY_TOTAL];
    B8          currKeyState[NK_KEY_TOTAL];
    S32         viewportX;
    S32         viewportY;
    S32         viewportW;
    S32         viewportH;
};

NKAPI void nkPlaySound (nkCONTEXT* nokia, nkSND snd)
{
    NK_ASSERT(nokia && snd >= 0 && snd < NK_SND_TOTAL);

    static const char* SOUNDS[NK_SND_TOTAL] =
    {
        NULL,
        "SNDBANK/BLIP01.WAV",
        "SNDBANK/BLIP02.WAV",
        "SNDBANK/BLIP03.WAV",
        "SNDBANK/BLIP04.WAV",
        "SNDBANK/BLIP05.WAV",
        "SNDBANK/BLIP06.WAV",
        "SNDBANK/BLIP07.WAV",
        "SNDBANK/BLIP08.WAV",
        "SNDBANK/BLIP09.WAV",
        "SNDBANK/BLIP10.WAV",
        "SNDBANK/BLIP11.WAV",
        "SNDBANK/BLIP12.WAV",
        "SNDBANK/BLIP13.WAV",
        "SNDBANK/BLIP14.WAV",
        "SNDBANK/C5.WAV",
        "SNDBANK/CRUST.WAV",
        "SNDBANK/GOOD01.WAV",
        "SNDBANK/GOOD02.WAV",
        "SNDBANK/GOOD03.WAV",
        "SNDBANK/HIT01.WAV",
        "SNDBANK/HIT02.WAV",
        "SNDBANK/HIT03.WAV",
        "SNDBANK/HIT04.WAV",
        "SNDBANK/HIT06.WAV",
        "SNDBANK/JINGLE.WAV",
        "SNDBANK/MELODY.WAV",
        "SNDBANK/NEGTIV01.WAV",
        "SNDBANK/NEGTIV02.WAV",
        "SNDBANK/ODD01.WAV",
        "SNDBANK/ODD02.WAV",
        "SNDBANK/ODD03.WAV",
        "SNDBANK/ODD04.WAV",
        "SNDBANK/RING.WAV",
        "SNDBANK/SNDTEST.WAV",
        "SNDBANK/UNREAL.WAV",
    };

    nokia->sound = SOUNDS[snd];
}

NKAPI void nkSetSprite (nkCONTEXT* nokia, S32 x, S32 y, U8 slot, U8 index)
{
    NK_ASSERT(nokia && slot < NK_MAX_SPRITES);
    nkSPRITE* spr = &nokia->spriteList[slot];
    spr->x = x, spr->y = y, spr->index = index;
}

NKAPI void nkSetText (nkCONTEXT* nokia, S32 tileX, S32 tileY, const char* text, ...)
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

    for (const char* c=textBuffer; *c; ++c)
    {
        if (tileX >= 0 && tileX < NK_SCREEN_W_TILES &&
            tileY >= 0 && tileY < NK_SCREEN_H_TILES)
        {
            nokia->textMap[tileY*NK_SCREEN_W_TILES+tileX] = *c;

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
    return (nokia->currKeyState[key]);
}
NKAPI B8 nkKeyDown (nkCONTEXT* nokia, nkKEY key)
{
    NK_ASSERT(nokia && key >= 0 && key < NK_KEY_TOTAL);
    return (!nokia->currKeyState[key]);
}

// Do not call this function! It is used internally to convert the internal tile, text,
// and sprite data into a screen of pixels that can then be drawn to the user's display.
NKAPI void nkDrawFrame (nkCONTEXT* nokia)
{
    NK_ASSERT(nokia);

    for (S32 i=0, n=(NK_SCREEN_W*NK_SCREEN_H); i<n; ++i)
    {
        nokia->screen.pixels[i] = NK_COLOR_0;
    }

    for (int iy=0; iy<NK_SCREEN_H_TILES; ++iy)
    {
        for (int ix=0; ix<NK_SCREEN_W_TILES; ++ix)
        {
            if (nokia->textMap[iy*NK_SCREEN_W_TILES+ix])
            {
                for (int i=0; i<NK_TILE_H; ++i)
                {
                    U8 row = NK_FONT[(nokia->textMap[iy*NK_SCREEN_W_TILES+ix]*NK_TILE_H)+i];
                    for (int j=0; j<NK_TILE_W; ++j)
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
                    for (int i=0; i<NK_TILE_H; ++i)
                    {
                        U8 row = NK_TILESET[(nokia->tileMap[iy*NK_SCREEN_W_TILES+ix]*NK_TILE_H)+i];
                        for (int j=0; j<NK_TILE_W; ++j)
                        {
                            if (row & (1 << (7-j)))
                            {
                                nokia->screen.pixels[(iy*NK_TILE_H)*NK_SCREEN_W+(ix*NK_TILE_W)] = NK_COLOR_1;
                            }
                        }
                    }
                }
            }
        }
    }

    for (int i=0; i<NK_MAX_SPRITES; ++i)
    {
        if (nokia->spriteList[i].index)
        {
            // @Incomplete: ...
        }
    }
}
