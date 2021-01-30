void nkGameUpdate (nkCONTEXT* nokia)
{
    if (nkKeyPressed(nokia, NK_KEY_SPACE))
    {
        nkPlaySound(nokia, NK_SND_JINGLE);
    }

    nkSetText(nokia, 0,0, "HELLO NOKIA!");
    // nkSetText(nokia, 0,1, "HELLO NOKIA!");
    // nkSetText(nokia, 0,2, "HELLO NOKIA!");
    // nkSetText(nokia, 0,3, "HELLO NOKIA!");
    // nkSetText(nokia, 0,4, "HELLO NOKIA!");
    // nkSetText(nokia, 0,5, "HELLO NOKIA!");

    static S32 sprX = 12;
    static S32 sprY = 20;

    if (nkKeyDown(nokia, NK_KEY_W)) sprY -= 2;
    if (nkKeyDown(nokia, NK_KEY_S)) sprY += 2;
    if (nkKeyDown(nokia, NK_KEY_A)) sprX -= 2;
    if (nkKeyDown(nokia, NK_KEY_D)) sprX += 2;

    nkSetSprite(nokia, sprX,sprY, 0,1);
}
