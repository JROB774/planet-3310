#include <emscripten.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "NOKIA.C"
#include "GAME.C"

typedef struct
{
    SDL_Window*   window;
    SDL_Renderer* renderer;
    SDL_Surface*  screen;
    SDL_Texture*  target;
    Mix_Chunk*    sounds[NK_SND_TOTAL];
    nkCONTEXT*    nokia;

} SDLContext;

void SDLFatalError (const char* msg)
{
    fprintf(stderr, "%s (%s)\n", msg, SDL_GetError());
    exit(1); // Failure
}
void SDLMixFatalError (const char* msg)
{
    fprintf(stderr, "%s (%s)\n", msg, Mix_GetError());
    exit(1); // Failure
}

void SDLMainLoop (void* userdata)
{
    SDLContext* context = (SDLContext*)userdata;

    SDL_PumpEvents();

    memcpy(context->nokia->prevKeyState, context->nokia->currKeyState, sizeof(context->nokia->prevKeyState));
    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    context->nokia->currKeyState[NK_KEY_Q    ] = keyState[SDL_SCANCODE_Q];
    context->nokia->currKeyState[NK_KEY_W    ] = keyState[SDL_SCANCODE_W];
    context->nokia->currKeyState[NK_KEY_E    ] = keyState[SDL_SCANCODE_E];
    context->nokia->currKeyState[NK_KEY_A    ] = keyState[SDL_SCANCODE_A];
    context->nokia->currKeyState[NK_KEY_S    ] = keyState[SDL_SCANCODE_S];
    context->nokia->currKeyState[NK_KEY_D    ] = keyState[SDL_SCANCODE_D];
    context->nokia->currKeyState[NK_KEY_Z    ] = keyState[SDL_SCANCODE_Z];
    context->nokia->currKeyState[NK_KEY_X    ] = keyState[SDL_SCANCODE_X];
    context->nokia->currKeyState[NK_KEY_C    ] = keyState[SDL_SCANCODE_C];
    context->nokia->currKeyState[NK_KEY_SPACE] = keyState[SDL_SCANCODE_SPACE];
    context->nokia->currKeyState[NK_KEY_COMMA] = keyState[SDL_SCANCODE_COMMA];
    context->nokia->currKeyState[NK_KEY_POINT] = keyState[SDL_SCANCODE_PERIOD];

    nkBeginFrame(context->nokia);
    nkGameUpdate(context->nokia);
    nkEndFrame(context->nokia);

    if (context->nokia->sound)
    {
        Mix_PlayChannel(0, context->sounds[context->nokia->sound], 0);
        context->nokia->sound = NK_SND_NONE; // Reset the sound after playing it.
    }

    SDL_SetRenderDrawColor(context->renderer, 0,0,0,255);
    SDL_RenderClear(context->renderer);
    SDL_UpdateTexture(context->target, NULL, context->screen->pixels, context->screen->pitch);
    SDL_RenderCopy(context->renderer, context->target, NULL, NULL);
    SDL_RenderPresent(context->renderer);
}

int main (int argc, char** argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDLFatalError("Failed to initialize SDL2!");
    }
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 2048) != 0)
    {
        SDLMixFatalError("Failed to open audio device!");
    }

    Mix_AllocateChannels(1);

    int windowWidth = NK_SCREEN_W*8;
    int windowHeight = NK_SCREEN_H*8;

    SDL_Window* window = SDL_CreateWindow("NOKIA", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, windowWidth,windowHeight, SDL_WINDOW_SHOWN);
    if (!window)
    {
        SDLFatalError("Failed to create the window!");
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        SDLFatalError("Failed to create the renderer!");
    }

    Uint32 pixelFormat = SDL_GetWindowPixelFormat(window);
    if (pixelFormat == SDL_PIXELFORMAT_UNKNOWN)
    {
        SDLFatalError("Failed to retrieve window pixel format!");
    }

    // Convert the window's pixel format into a mask usable with SDL_CreateRGBSurface.
    Uint32 r,g,b,a;
    int bpp; // We don't use this but SDL needs us to pass it.
    if (!SDL_PixelFormatEnumToMasks(pixelFormat, &bpp, &r,&g,&b,&a))
    {
        SDLFatalError("Failed to convert format to mask!");
    }
    SDL_Surface* screen = SDL_CreateRGBSurface(0, NK_SCREEN_W,NK_SCREEN_H, 32, r,g,b,a); // Our screen pixels.
    if (!screen)
    {
        SDLFatalError("Failed to create screen buffer!");
    }
    SDL_Texture* target = SDL_CreateTexture(renderer, pixelFormat,
        SDL_TEXTUREACCESS_STREAMING, NK_SCREEN_W,NK_SCREEN_H);
    if (!target)
    {
        SDLFatalError("Failed to create screen texture!");
    }

    nkCONTEXT nokia;
    SDL_zero(nokia);
    nokia.screen.width = NK_SCREEN_W;
    nokia.screen.height = NK_SCREEN_H;
    nokia.screen.bits = 32;
    nokia.screen.pixels = (U32*)screen->pixels;

    SDLContext context;
    context.window = window;
    context.renderer = renderer;
    context.screen = screen;
    context.target = target;
    context.nokia = &nokia;

    context.sounds[NK_SND_NONE    ] = NULL;
    context.sounds[NK_SND_BLIP01  ] = Mix_LoadWAV("bin/web/SNDBANK/BLIP01.WAV"  );
    context.sounds[NK_SND_BLIP02  ] = Mix_LoadWAV("bin/web/SNDBANK/BLIP02.WAV"  );
    context.sounds[NK_SND_BLIP03  ] = Mix_LoadWAV("bin/web/SNDBANK/BLIP03.WAV"  );
    context.sounds[NK_SND_BLIP04  ] = Mix_LoadWAV("bin/web/SNDBANK/BLIP04.WAV"  );
    context.sounds[NK_SND_BLIP05  ] = Mix_LoadWAV("bin/web/SNDBANK/BLIP05.WAV"  );
    context.sounds[NK_SND_BLIP06  ] = Mix_LoadWAV("bin/web/SNDBANK/BLIP06.WAV"  );
    context.sounds[NK_SND_BLIP07  ] = Mix_LoadWAV("bin/web/SNDBANK/BLIP07.WAV"  );
    context.sounds[NK_SND_BLIP08  ] = Mix_LoadWAV("bin/web/SNDBANK/BLIP08.WAV"  );
    context.sounds[NK_SND_BLIP09  ] = Mix_LoadWAV("bin/web/SNDBANK/BLIP09.WAV"  );
    context.sounds[NK_SND_BLIP10  ] = Mix_LoadWAV("bin/web/SNDBANK/BLIP10.WAV"  );
    context.sounds[NK_SND_BLIP11  ] = Mix_LoadWAV("bin/web/SNDBANK/BLIP11.WAV"  );
    context.sounds[NK_SND_BLIP12  ] = Mix_LoadWAV("bin/web/SNDBANK/BLIP12.WAV"  );
    context.sounds[NK_SND_BLIP13  ] = Mix_LoadWAV("bin/web/SNDBANK/BLIP13.WAV"  );
    context.sounds[NK_SND_BLIP14  ] = Mix_LoadWAV("bin/web/SNDBANK/BLIP14.WAV"  );
    context.sounds[NK_SND_C5      ] = Mix_LoadWAV("bin/web/SNDBANK/C5.WAV"      );
    context.sounds[NK_SND_CRUST   ] = Mix_LoadWAV("bin/web/SNDBANK/CRUST.WAV"   );
    context.sounds[NK_SND_GOOD01  ] = Mix_LoadWAV("bin/web/SNDBANK/GOOD01.WAV"  );
    context.sounds[NK_SND_GOOD02  ] = Mix_LoadWAV("bin/web/SNDBANK/GOOD02.WAV"  );
    context.sounds[NK_SND_GOOD03  ] = Mix_LoadWAV("bin/web/SNDBANK/GOOD03.WAV"  );
    context.sounds[NK_SND_HIT01   ] = Mix_LoadWAV("bin/web/SNDBANK/HIT01.WAV"   );
    context.sounds[NK_SND_HIT02   ] = Mix_LoadWAV("bin/web/SNDBANK/HIT02.WAV"   );
    context.sounds[NK_SND_HIT03   ] = Mix_LoadWAV("bin/web/SNDBANK/HIT03.WAV"   );
    context.sounds[NK_SND_HIT04   ] = Mix_LoadWAV("bin/web/SNDBANK/HIT04.WAV"   );
    context.sounds[NK_SND_HIT05   ] = Mix_LoadWAV("bin/web/SNDBANK/HIT05.WAV"   );
    context.sounds[NK_SND_HIT06   ] = Mix_LoadWAV("bin/web/SNDBANK/HIT06.WAV"   );
    context.sounds[NK_SND_JINGLE  ] = Mix_LoadWAV("bin/web/SNDBANK/JINGLE.WAV"  );
    context.sounds[NK_SND_MELODY  ] = Mix_LoadWAV("bin/web/SNDBANK/MELODY.WAV"  );
    context.sounds[NK_SND_NEGTIV01] = Mix_LoadWAV("bin/web/SNDBANK/NEGTIV01.WAV");
    context.sounds[NK_SND_NEGTIV02] = Mix_LoadWAV("bin/web/SNDBANK/NEGTIV02.WAV");
    context.sounds[NK_SND_ODD01   ] = Mix_LoadWAV("bin/web/SNDBANK/ODD01.WAV"   );
    context.sounds[NK_SND_ODD02   ] = Mix_LoadWAV("bin/web/SNDBANK/ODD02.WAV"   );
    context.sounds[NK_SND_ODD03   ] = Mix_LoadWAV("bin/web/SNDBANK/ODD03.WAV"   );
    context.sounds[NK_SND_ODD04   ] = Mix_LoadWAV("bin/web/SNDBANK/ODD04.WAV"   );
    context.sounds[NK_SND_RING    ] = Mix_LoadWAV("bin/web/SNDBANK/RING.WAV"    );
    context.sounds[NK_SND_SNDTEST ] = Mix_LoadWAV("bin/web/SNDBANK/SNDTEST.WAV" );
    context.sounds[NK_SND_UNREAL  ] = Mix_LoadWAV("bin/web/SNDBANK/UNREAL.WAV"  );

    nkGameStartup(&context.nokia);

    emscripten_set_main_loop_arg(SDLMainLoop, &context, 15, 1);

    return 0;
}
