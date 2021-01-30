#include <stdint.h>
#include <string.h>

#define NOKIA_SCREEN_W 84
#define NOKIA_SCREEN_H 48

#define NOKIA_FALSE 0
#define NOKIA_TRUE  1

#define NOKIA_STRUCT(name) typedef struct name##_T name; struct name##_T
#define NOKIA_ENUM(name)   typedef enum   name##_T name; enum   name##_T

#ifdef NOKIA_DEBUG
#include <assert.h>
#define NOKIA_ASSERT(e) assert(e)
#else
#define NOKIA_ASSERT(e) (void)(0)
#endif

#define NOKIA_ZERO_MEM(    x)   memset(&(x), 0, sizeof( (x)))
#define NOKIA_ZERO_MEM_PTR(x)   memset( (x), 0, sizeof(*(x)))
#define NOKIA_CLAMP(x,low,high) (((x)>(high))?(high):(((x)<(low))?(low):(x)))
#define NOKIA_ABS(a)            (((a)<0)?-(a):(a))
#define NOKIA_SWAP(x,y,t)       do { t tmp__ = x; x = y; y = tmp__; } while (0)
#define NOKIA_MIN(a,b)          (((a)<(b))?(a):(b))
#define NOKIA_MAX(a,b)          (((a)>(b))?(a):(b))
#define NOKIA_CAST(t,x)         ((t)(x))

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

NOKIA_ENUM(KEY)
{
    NOKIA_KEY_Q,
    NOKIA_KEY_W,
    NOKIA_KEY_E,
    NOKIA_KEY_A,
    NOKIA_KEY_S,
    NOKIA_KEY_D,
    NOKIA_KEY_Z,
    NOKIA_KEY_X,
    NOKIA_KEY_C,
    NOKIA_KEY_SPACE,
    NOKIA_KEY_COMMA,
    NOKIA_KEY_POINT,
    NOKIA_KEY_TOTAL
};

NOKIA_STRUCT(SCREEN)
{
    S32  width;
    S32  height;
    S32  bits;
    U32* pixels;
};

NOKIA_STRUCT(NOKIA)
{
    SCREEN screen;
    B8     prevKeyState[NOKIA_KEY_TOTAL];
    B8     currKeyState[NOKIA_KEY_TOTAL];
    S32    viewportX;
    S32    viewportY;
    S32    viewportW;
    S32    viewportH;
};
