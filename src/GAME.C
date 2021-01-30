void nkGameUpdate (nkCONTEXT* nokia)
{
    if (nkKeyPressed(nokia, NK_KEY_SPACE))
    {
        nkPlaySound(nokia, NK_SND_JINGLE);
    }

    nkSetText(nokia, 0,0, "HELLO NOKIA!");
    nkSetText(nokia, 0,1, "HELLO NOKIA!");
    nkSetText(nokia, 0,2, "HELLO NOKIA!");
    nkSetText(nokia, 0,3, "HELLO NOKIA!");
    nkSetText(nokia, 0,4, "HELLO NOKIA!");
    nkSetText(nokia, 0,5, "HELLO NOKIA!");
}
