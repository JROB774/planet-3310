#include <stdint.h>
#include <assert.h>
#include <string.h>

#define NK_SCREEN_W 84
#define NK_SCREEN_H 48

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

NK_STRUCT(nkSCREEN)
{
    S32  width;
    S32  height;
    S32  bits;
    U32* pixels;
};

NK_STRUCT(nkCONTEXT)
{
    nkSCREEN    screen;
    const char* snd;
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

    nokia->snd = SOUNDS[snd];
}

NKAPI void nkDrawPoint (nkCONTEXT* nokia, S32 x, S32 y)
{
    NK_ASSERT(nokia);
    if (x < 0 || x >= NK_SCREEN_W || y < 0 || y >= NK_SCREEN_H) return;
    nokia->screen.pixels[y*NK_SCREEN_W+x] = NK_COLOR_1;
}
NKAPI void nkDrawLine (nkCONTEXT* nokia, S32 x1, S32 y1, S32 x2, S32 y2)
{
    NK_ASSERT(nokia);

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
    NK_ASSERT(nokia);

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
    NK_ASSERT(nokia);

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
    NK_ASSERT(nokia);

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
    return (nokia->currKeyState[key]);
}
NKAPI B8 nkKeyDown (nkCONTEXT* nokia, nkKEY key)
{
    NK_ASSERT(nokia && key >= 0 && key < NK_KEY_TOTAL);
    return (!nokia->currKeyState[key]);
}
