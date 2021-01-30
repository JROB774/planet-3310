void NkGameUpdate (nkCONTEXT* nokia)
{
    if (nkKeyPressed(nokia, NK_KEY_SPACE))
    {
        nkPlaySound(nokia, NK_SND_JINGLE);
    }
}
