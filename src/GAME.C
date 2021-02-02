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

    static S32 sprX = 12;
    static S32 sprY = 20;

    if (nkKeyDown(nokia, NK_KEY_W)) sprY -= 2;
    if (nkKeyDown(nokia, NK_KEY_S)) sprY += 2;
    if (nkKeyDown(nokia, NK_KEY_A)) sprX -= 2;
    if (nkKeyDown(nokia, NK_KEY_D)) sprX += 2;

    nkSetSprite(nokia, sprX,sprY, 0,1);
}
