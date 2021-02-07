#define SPR_PLAYER0  0x00
#define SPR_PBULLET  0x04
#define SPR_EXPLODE  0x06
#define SPR_MONPAWN  0x08
#define SPR_MONSINE  0x0C
#define SPR_MONBOOM  0x10
#define SPR_MONBLOB  0x1C

#define ENT_EXPLODE  0x00
#define ENT_PBULLET  0x01
#define ENT_PLAYER   0x02
#define ENT_MONPAWN  0x03
#define ENT_MONSINE  0x04
#define ENT_MONBLOB  0x05
#define ENT_MONBOOM  0x06

#define DIR_N        0x00
#define DIR_NE       0x01
#define DIR_E        0x02
#define DIR_SE       0x03
#define DIR_S        0x04
#define DIR_SW       0x05
#define DIR_W        0x06
#define DIR_NW       0x07

#define GMODE_MENU   0x00
#define GMODE_SCORES 0x01
#define GMODE_GAME   0x02

#define MENU_OPSTART 0x00
#define MENU_OPSCORE 0x01
#define MENU_OPEXIT  0x02

#define NUM_ENTITIES 0xFF

typedef struct
{
    S8 x;
    S8 y;
    U8 hits;
    U8 type;
    U8 spr;
    U8 sprW;
    U8 sprH;
    U8 frame;
    U8 frameNum;
    U8 timer;
    B8 active;
    U8 extra;

} ENTITY;

static ENTITY  gEntities[NUM_ENTITIES];
static ENTITY* gPlayer;
static U32     gSpawnPawnCounter;
static U32     gSpawnSineCounter;
static U32     gSpawnBoomCounter;
static B8      gPaused;
static S32     gScore;
static U8      gGameMode;

U32 GetEntityTypeCount (U8 type)
{
    U32 count = 0;
    for (S32 i=0; i<NUM_ENTITIES; ++i)
    {
        ENTITY* e = &gEntities[i];
        if (e->active && e->type == type)
        {
            count++;
        }
    }
    return count;
}

B8 CheckCollision (ENTITY* a, ENTITY* b)
{
    return ((a->x < b->x+(b->sprW*NK_TILE_W)) &&
            (a->x+(a->sprW*NK_TILE_W) > b->x) &&
            (a->y < b->y+(b->sprH*NK_TILE_H)) &&
            (a->y+(a->sprH*NK_TILE_H) > b->y));
}

//
// ENT_EXPLODE
//
void SpawnExplode (S32 x, S32 y)
{
    for (S32 i=1; i<NUM_ENTITIES; ++i)
    {
        ENTITY* e = &gEntities[i];
        if (!e->active)
        {
            e->x        = x;
            e->y        = y;
            e->hits     = 0;
            e->type     = ENT_EXPLODE;
            e->spr      = SPR_EXPLODE;
            e->sprW     = 1;
            e->sprH     = 1;
            e->frame    = 0;
            e->frameNum = 2;
            e->timer    = 0;
            e->active   = NK_TRUE;
            break;
        }
    }
}
void UpdateExplode (nkCONTEXT* nokia, ENTITY* e)
{
    e->timer++;
    if (e->timer >= 4)
    {
        e->active = NK_FALSE;
    }
}

//
// ENT_PBULLET
//
void SpawnBullet (S32 x, S32 y)
{
    for (S32 i=1; i<NUM_ENTITIES; ++i)
    {
        ENTITY* e = &gEntities[i];
        if (!e->active)
        {
            e->x        = x;
            e->y        = y;
            e->hits     = 0;
            e->type     = ENT_PBULLET;
            e->spr      = SPR_PBULLET;
            e->sprW     = 1;
            e->sprH     = 1;
            e->frame    = 0;
            e->frameNum = 1;
            e->timer    = 0;
            e->active   = NK_TRUE;
            break;
        }
    }
}
void UpdateBullet (nkCONTEXT* nokia, ENTITY* e)
{
    e->x += 12;
    if (e->x >= NK_SCREEN_W)
    {
        e->active = NK_FALSE;
    }
}

//
// ENT_PLAYER
//
void SpawnPlayer (S32 x, S32 y)
{
    gPlayer->x        = x;
    gPlayer->y        = y;
    gPlayer->hits     = 1;
    gPlayer->type     = ENT_PLAYER;
    gPlayer->spr      = SPR_PLAYER0;
    gPlayer->sprW     = 2;
    gPlayer->sprH     = 1;
    gPlayer->frame    = 0;
    gPlayer->frameNum = 2;
    gPlayer->timer    = 0;
    gPlayer->active   = NK_TRUE;
}
void UpdatePlayer (nkCONTEXT* nokia, ENTITY* e)
{
    static S8 bulletCooldown = 0;

    U8 playerSpeed = 3;

    if (nkKeyDown(nokia, NK_KEY_W)) gPlayer->y -= playerSpeed;
    if (nkKeyDown(nokia, NK_KEY_A)) gPlayer->x -= playerSpeed;
    if (nkKeyDown(nokia, NK_KEY_S)) gPlayer->y += playerSpeed;
    if (nkKeyDown(nokia, NK_KEY_D)) gPlayer->x += playerSpeed;

    S32 w = (gPlayer->sprW * NK_TILE_W);
    S32 h = (gPlayer->sprH * NK_TILE_H);

    if (gPlayer->x < 0) e->x = 0;
    if (gPlayer->x+w > NK_SCREEN_W) gPlayer->x = NK_SCREEN_W-w;
    if (gPlayer->y < 0) e->y = 0;
    if (gPlayer->y+h > NK_SCREEN_H) gPlayer->y = NK_SCREEN_H-h;

    // Fire bullets.
    if (bulletCooldown == 0)
    {
        if (nkKeyDown(nokia, NK_KEY_SPACE))
        {
            SpawnBullet(gPlayer->x+(NK_TILE_W*2)-3,gPlayer->y);
            bulletCooldown = 2;
        }
    }
    else
    {
        bulletCooldown--;
    }

    for (S32 i=1; i<NUM_ENTITIES; ++i)
    {
        ENTITY* e2 = &gEntities[i];
        if (e2->active && (e2->type == ENT_MONPAWN ||
                           e2->type == ENT_MONSINE ||
                           e2->type == ENT_MONBLOB ||
                           e2->type == ENT_MONBOOM))
        {
            if (CheckCollision(e,e2))
            {
                gPlayer->active = NK_FALSE;
                SpawnExplode(gPlayer->x+(NK_TILE_W/2),gPlayer->y);
                nkPlaySound(nokia, NK_SND_NEGTIV01);
                break;
            }
        }
    }
}

//
// ENT_MONPAWN
//
void SpawnMonsterPawn (S32 x, S32 y)
{
    for (S32 i=1; i<NUM_ENTITIES; ++i)
    {
        ENTITY* e = &gEntities[i];
        if (!e->active)
        {
            e->x        = x;
            e->y        = y;
            e->hits     = 1;
            e->type     = ENT_MONPAWN;
            e->spr      = SPR_MONPAWN;
            e->sprW     = 2;
            e->sprH     = 1;
            e->frame    = 0;
            e->frameNum = 2;
            e->timer    = 0;
            e->active   = NK_TRUE;
            break;
        }
    }
}
void UpdateMonsterPawn (nkCONTEXT* nokia, ENTITY* e)
{
    e->x -= 4;
    if (e->x+(e->sprW*NK_TILE_W) < 0)
    {
        e->active = NK_FALSE;
        if (gPlayer->active)
        {
            gScore -= 10;
        }
    }

    for (S32 i=1; i<NUM_ENTITIES; ++i)
    {
        ENTITY* e2 = &gEntities[i];
        if (e2->active && e2->type == ENT_PBULLET)
        {
            if (CheckCollision(e,e2))
            {
                if (gPlayer->active) // So it doesn't override the death sound.
                {
                    nkPlaySound(nokia, NK_SND_HIT01);
                }
                e->hits--;
                if (e->hits <= 0)
                {
                    e->active = NK_FALSE;
                    SpawnExplode(e->x+(NK_TILE_W/2),e->y);
                    gScore += 10;
                    break;
                }
            }
        }
    }
}

//
// ENT_MONSINE
//
void SpawnMonsterSine (S32 x, S32 y)
{
    for (S32 i=1; i<NUM_ENTITIES; ++i)
    {
        ENTITY* e = &gEntities[i];
        if (!e->active)
        {
            e->x        = x;
            e->y        = y;
            e->hits     = 1;
            e->type     = ENT_MONSINE;
            e->spr      = SPR_MONSINE;
            e->sprW     = 2;
            e->sprH     = 1;
            e->frame    = 0;
            e->frameNum = 2;
            e->timer    = 0;
            e->active   = NK_TRUE;
            break;
        }
    }
}
void UpdateMonsterSine (nkCONTEXT* nokia, ENTITY* e)
{
    e->x -= 7;
    e->y = nkSinRange(0, NK_SCREEN_H-NK_TILE_H, NK_CAST(F32,nokia->frame)/4);
    if (e->x+(e->sprW*NK_TILE_W) < 0)
    {
        e->active = NK_FALSE;
        if (gPlayer->active)
        {
            gScore -= 10;
        }
    }

    for (S32 i=1; i<NUM_ENTITIES; ++i)
    {
        ENTITY* e2 = &gEntities[i];
        if (e2->active && e2->type == ENT_PBULLET)
        {
            if (CheckCollision(e,e2))
            {
                if (gPlayer->active) // So it doesn't override the death sound.
                {
                    nkPlaySound(nokia, NK_SND_HIT02);
                }
                e->hits--;
                if (e->hits <= 0)
                {
                    e->active = NK_FALSE;
                    SpawnExplode(e->x+(NK_TILE_W/2),e->y);
                    gScore += 30;
                    break;
                }
            }
        }
    }
}

//
// ENT_MONBLOB
//
void SpawnMonsterBlob (S32 x, S32 y, U8 dir)
{
    for (S32 i=1; i<NUM_ENTITIES; ++i)
    {
        ENTITY* e = &gEntities[i];
        if (!e->active)
        {
            e->x        = x;
            e->y        = y;
            e->hits     = 0;
            e->type     = ENT_MONBLOB;
            e->spr      = SPR_MONBLOB;
            e->sprW     = 1;
            e->sprH     = 1;
            e->frame    = 0;
            e->frameNum = 4;
            e->timer    = 0;
            e->active   = NK_TRUE;
            e->extra    = dir;
            break;
        }
    }
}
void UpdateMonsterBlob (nkCONTEXT* nokia, ENTITY* e)
{
    if ((e->x >= NK_SCREEN_W)          ||
        (e->x+(e->sprW*NK_TILE_W) < 0) ||
        (e->y >= NK_SCREEN_H)          ||
        (e->y+(e->sprH*NK_TILE_H) < 0))
    {
        e->active = NK_FALSE;
    }

    switch (e->extra)
    {
        case (DIR_N): e->y -= 4; break;
        case (DIR_E): e->x += 4; break;
        case (DIR_S): e->y += 4; break;
        case (DIR_W): e->x -= 4; break;
    }
}

//
// ENT_MONBOOM
//
void SpawnMonsterBoom (S32 x, S32 y)
{
    for (S32 i=1; i<NUM_ENTITIES; ++i)
    {
        ENTITY* e = &gEntities[i];
        if (!e->active)
        {
            e->x        = x;
            e->y        = y;
            e->hits     = 15;
            e->type     = ENT_MONBOOM;
            e->spr      = SPR_MONBOOM;
            e->sprW     = 2;
            e->sprH     = 2;
            e->frame    = 0;
            e->frameNum = 3;
            e->timer    = 0;
            e->active   = NK_TRUE;
            break;
        }
    }
}
void UpdateMonsterBoom (nkCONTEXT* nokia, ENTITY* e)
{
    e->x -= 1;
    if (e->x+(e->sprW*NK_TILE_W) < 0)
    {
        e->active = NK_FALSE;
        if (gPlayer->active)
        {
            gScore -= 100;
        }
    }

    for (S32 i=1; i<NUM_ENTITIES; ++i)
    {
        ENTITY* e2 = &gEntities[i];
        if (e2->active && e2->type == ENT_PBULLET)
        {
            if (CheckCollision(e,e2))
            {
                if (gPlayer->active) // So it doesn't override the death sound.
                {
                    nkPlaySound(nokia, NK_SND_HIT03);
                }
                e->hits--;
                if (e->hits <= 0)
                {
                    e->active = NK_FALSE;
                    SpawnExplode(e->x,          e->y);
                    SpawnExplode(e->x+NK_TILE_W,e->y);
                    SpawnExplode(e->x,          e->y+NK_TILE_H);
                    SpawnExplode(e->x+NK_TILE_W,e->y+NK_TILE_H);
                    SpawnMonsterBlob(e->x,e->y, DIR_N);
                    SpawnMonsterBlob(e->x,e->y, DIR_E);
                    SpawnMonsterBlob(e->x,e->y, DIR_S);
                    SpawnMonsterBlob(e->x,e->y, DIR_W);
                    gScore += 100;
                    break;
                }
            }
        }
    }
}

void StartGame (nkCONTEXT* nokia)
{
    gGameMode = GMODE_GAME;

    NK_ZERO_MEM_ARR(gEntities);

    nkClearTiles(nokia);
    nkClearText(nokia);

    gPlayer = gEntities;
    SpawnPlayer(2,(NK_SCREEN_H-NK_TILE_H)/2);

    gScore = 0;

    gSpawnPawnCounter =  20;
    gSpawnSineCounter = 240;
    gSpawnBoomCounter = 500;
}

void UpdateMenu (nkCONTEXT* nokia)
{
    static S8 option = MENU_OPSTART;

    nkClearText(nokia);

    nkSetText(nokia, 0,0, NK_FALSE, "SPACE ATTACK");
    nkSetText(nokia, 3,3, (option == MENU_OPSTART), "ATTACK");
    nkSetText(nokia, 3,4, (option == MENU_OPSCORE), "SCORES");
    nkSetText(nokia, 4,5, (option == MENU_OPEXIT ), "EXIT"  );

    // Menu controls.
    if (nkKeyPressed(nokia, NK_KEY_W))
    {
        option--;
        if (option < MENU_OPSTART)
        {
            option = MENU_OPEXIT;
        }
    }
    if (nkKeyPressed(nokia, NK_KEY_S))
    {
        option++;
        if (option > MENU_OPEXIT)
        {
            option = MENU_OPSTART;
        }
    }
    if (nkKeyPressed(nokia, NK_KEY_SPACE))
    {
        nkPlaySound(nokia, NK_SND_BLIP05);
        switch (option)
        {
            case (MENU_OPSTART): StartGame(nokia);         break;
            case (MENU_OPSCORE): gGameMode = GMODE_SCORES; break;
            case (MENU_OPEXIT ): nkExit(nokia);            break;
        }
    }
    if (nkKeyPressed(nokia, NK_KEY_ESCAPE))
    {
        nkExit(nokia);
    }
}

void UpdateScores (nkCONTEXT* nokia)
{
    nkClearText(nokia);

    nkSetText(nokia, 3,0, NK_FALSE, "000000");
    nkSetText(nokia, 3,1, NK_FALSE, "000000");
    nkSetText(nokia, 3,2, NK_FALSE, "000000");
    nkSetText(nokia, 3,3, NK_FALSE, "000000");
    nkSetText(nokia, 3,4, NK_FALSE, "000000");
    nkSetText(nokia, 3,5, NK_FALSE, "000000");

    // Score controls.
    if (nkKeyPressed(nokia, NK_KEY_SPACE) ||
        nkKeyPressed(nokia, NK_KEY_ESCAPE))
    {
        nkPlaySound(nokia, NK_SND_BLIP05);
        gGameMode = GMODE_MENU;
    }
}

void UpdateGame (nkCONTEXT* nokia)
{
    // Global game controls.
    if (gPlayer->active)
    {
        if (nkKeyPressed(nokia, NK_KEY_ESCAPE))
        {
            nkPlaySound(nokia, NK_SND_BLIP06);
            gPaused = !gPaused;
            nkClearText(nokia);
            if (gPaused)
            {
                nkSetText(nokia, 3,2, NK_TRUE, "PAUSED");
            }
        }
    }
    else
    {
        nkClearText(nokia);
        nkSetText(nokia, 3,2, NK_TRUE, "RETRY?");

        if (nkKeyPressed(nokia, NK_KEY_SPACE))
        {
            nkPlaySound(nokia, NK_SND_BLIP05);
            StartGame(nokia);
            return;
        }
        if (nkKeyPressed(nokia, NK_KEY_ESCAPE))
        {
            nkPlaySound(nokia, NK_SND_BLIP05);
            gGameMode = GMODE_MENU;
            return;
        }
    }

    if (gPaused) return;

    // Spawn monsters.
    if (gPlayer->active)
    {
        if (gSpawnPawnCounter == 0) // ENT_MONPAWN
        {
            SpawnMonsterPawn(NK_SCREEN_W, nkRandomRangeS32(0,(NK_SCREEN_H-NK_TILE_H)));
            gSpawnPawnCounter = nkRandomRangeS32(4,8);
        }
        else
        {
            gSpawnPawnCounter--;
        }
        if (gSpawnSineCounter == 0) // ENT_MONSINE
        {
            SpawnMonsterSine(NK_SCREEN_W, nkRandomRangeS32(0,(NK_SCREEN_H-NK_TILE_H)));
            gSpawnSineCounter = nkRandomRangeS32(8,15);
        }
        else
        {
            gSpawnSineCounter--;
        }
        if (gSpawnBoomCounter == 0) // ENT_MONBOOM
        {
            SpawnMonsterBoom(NK_SCREEN_W, nkRandomRangeS32(0,(NK_SCREEN_H-(NK_TILE_H*2))));
            gSpawnBoomCounter = nkRandomRangeS32(50,120);
        }
        else
        {
            gSpawnBoomCounter--;
        }
    }

    // Update entities.
    for (S32 i=0; i<NUM_ENTITIES; ++i)
    {
        ENTITY* e = &gEntities[i];
        if (e->active)
        {
            switch (e->type)
            {
                case (ENT_EXPLODE): UpdateExplode    (nokia, e); break;
                case (ENT_PBULLET): UpdateBullet     (nokia, e); break;
                case (ENT_PLAYER ): UpdatePlayer     (nokia, e); break;
                case (ENT_MONPAWN): UpdateMonsterPawn(nokia, e); break;
                case (ENT_MONSINE): UpdateMonsterSine(nokia, e); break;
                case (ENT_MONBLOB): UpdateMonsterBlob(nokia, e); break;
                case (ENT_MONBOOM): UpdateMonsterBoom(nokia, e); break;
            }
        }
    }

    // Draw entities.
    for (S32 i=0; i<NUM_ENTITIES; ++i)
    {
        ENTITY* e = &gEntities[i];
        if (e->active)
        {
            U8 sprCount = e->sprW * e->sprH;

            if (nokia->frame % 2 == 0) // Update animations.
            {
                e->frame++;
                if (e->frame >= e->frameNum)
                {
                    e->frame = 0;
                }
            }

            for (U8 iy=0; iy<e->sprH; ++iy)
            {
                for (U8 ix=0; ix<e->sprW; ++ix)
                {
                    U8 animOff = (e->frame*sprCount);
                    U8 sprOff = (iy*e->sprW+ix);

                    nkDrawSprite(nokia, e->x+(NK_TILE_W*ix), e->y+(NK_TILE_H*iy), e->spr+animOff+sprOff);
                }
            }
        }
    }

    // Update the score.
    if (gPlayer->active)
    {
        gScore = NK_CLAMP(gScore, 0, 999999);
        nkSetText(nokia, 3,0, NK_FALSE, "%06d", gScore);
    }
}

void nkGameStartup (nkCONTEXT* nokia)
{
    gGameMode = GMODE_MENU;
    nkSeedRandom();
}

void nkGameUpdate (nkCONTEXT* nokia)
{
    switch (gGameMode)
    {
        case (GMODE_MENU  ): UpdateMenu  (nokia); break;
        case (GMODE_SCORES): UpdateScores(nokia); break;
        case (GMODE_GAME  ): UpdateGame  (nokia); break;
    }
}
