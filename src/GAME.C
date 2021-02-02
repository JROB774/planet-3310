#include <math.h>

void nkGameUpdate (nkCONTEXT* nokia)
{
    if (nkKeyPressed(nokia, NK_KEY_SPACE))
    {
        nkPlaySound(nokia, NK_SND_JINGLE);
    }

    static B8 toggle0 = NK_TRUE;
    static B8 toggle1 = NK_FALSE;

    // nkSetText(nokia, 0,0, toggle0, "HELLO NOKIA!");
    // nkSetText(nokia, 0,1, toggle1, "HELLO NOKIA!");
    // nkSetText(nokia, 0,2, toggle0, "HELLO NOKIA!");
    // nkSetText(nokia, 0,3, toggle1, "HELLO NOKIA!");
    // nkSetText(nokia, 0,4, toggle0, "HELLO NOKIA!");
    // nkSetText(nokia, 0,5, toggle1, "HELLO NOKIA!");

    if (nokia->frame % 2 == 0) toggle0 = !toggle0;
    if (nokia->frame % 2 == 0) toggle1 = !toggle1;

    static F32 sprX = 12;
    static F32 sprY = 20;

    if (nkKeyDown(nokia, NK_KEY_W)) sprY -= 1.5f;
    if (nkKeyDown(nokia, NK_KEY_S)) sprY += 1.5f;
    if (nkKeyDown(nokia, NK_KEY_A)) sprX -= 1.5f;
    if (nkKeyDown(nokia, NK_KEY_D)) sprX += 1.5f;

    nkSetSprite(nokia, NK_ROUND(sprX),NK_ROUND(sprY), 0,1);
}
