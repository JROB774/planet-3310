#define SPR_PLAYER0  1
#define SPR_PLAYER1  2
#define SPR_PBULLET  5
#define SPR_MONSTR0  7
#define SPR_MONSTR1  8
#define SPR_EXPLODE 11

#define ENT_PLAYER  0
#define ENT_PBULLET 1
#define ENT_MONSTER 2
#define ENT_EXPLODE 3

#define NUM_ENTITIES 256

typedef struct
{
    S8 x;
    S8 y;
    U8 type;
    U8 spr;
    U8 sprW;
    U8 sprH;
    U8 frame;
    U8 frameNum;
    U8 timer;
    B8 active;

} ENTITY;

static ENTITY  gEntities[NUM_ENTITIES];
static ENTITY* gPlayer = gEntities;
static U32     gSpawnCounter = 0;
static U32     gScore = 0;

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
// ENT_PBULLET
//
void SpawnBullet (S32 x, S32 y)
{
    if (GetEntityTypeCount(ENT_PBULLET) < 5)
    {
        for (S32 i=0; i<NUM_ENTITIES; ++i)
        {
            ENTITY* e = &gEntities[i];
            if (!e->active)
            {
                e->x        = x;
                e->y        = y;
                e->type     = ENT_PBULLET;
                e->spr      = SPR_PBULLET;
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
    U8 playerSpeed = 3;

    if (nkKeyDown(nokia, NK_KEY_W)) gPlayer->y -= playerSpeed;
    if (nkKeyDown(nokia, NK_KEY_A)) gPlayer->x -= playerSpeed;
    if (nkKeyDown(nokia, NK_KEY_S)) gPlayer->y += playerSpeed;
    if (nkKeyDown(nokia, NK_KEY_D)) gPlayer->x += playerSpeed;

    if (nkKeyDown(nokia, NK_KEY_SPACE))
    {
        SpawnBullet(gPlayer->x+(NK_TILE_W*2)-3,gPlayer->y);
    }
}

//
// ENT_EXPLODE
//
void SpawnExplode (S32 x, S32 y)
{
    for (S32 i=0; i<NUM_ENTITIES; ++i)
    {
        ENTITY* e = &gEntities[i];
        if (!e->active)
        {
            e->x        = x;
            e->y        = y;
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
// ENT_MONSTER
//
void SpawnMonster (S32 x, S32 y)
{
    for (S32 i=0; i<NUM_ENTITIES; ++i)
    {
        ENTITY* e = &gEntities[i];
        if (!e->active)
        {
            e->x        = x;
            e->y        = y;
            e->type     = ENT_MONSTER;
            e->spr      = SPR_MONSTR0;
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
void UpdateMonster (nkCONTEXT* nokia, ENTITY* e)
{
    e->x -= 5;
    if (e->x+(e->sprW*NK_TILE_W) < 0)
    {
        e->active = NK_FALSE;
    }

    for (S32 i=0; i<NUM_ENTITIES; ++i)
    {
        ENTITY* e2 = &gEntities[i];
        if (e2->active && e2->type == ENT_PBULLET)
        {
            if (CheckCollision(e,e2))
            {
                e->active = NK_FALSE;
                SpawnExplode(e->x+(NK_TILE_W/2),e->y);
                nkPlaySound(nokia, NK_SND_HIT01);
                gScore += 10;
            }
        }
    }
}

void nkGameStartup (nkCONTEXT* nokia)
{
    SpawnPlayer(10,20);
}

void nkGameUpdate (nkCONTEXT* nokia)
{
    // Spawn enemies.
    if (gSpawnCounter == 0)
    {
        SpawnMonster(NK_SCREEN_W, rand()%(NK_SCREEN_H-NK_TILE_H));
        gSpawnCounter = rand() % 5;
    }
    else
    {
        gSpawnCounter--;
    }

    // Update entities.
    for (S32 i=0; i<NUM_ENTITIES; ++i)
    {
        ENTITY* e = &gEntities[i];
        if (e->active)
        {
            switch (e->type)
            {
                case (ENT_PLAYER ): UpdatePlayer (nokia, e); break;
                case (ENT_PBULLET): UpdateBullet (nokia, e); break;
                case (ENT_MONSTER): UpdateMonster(nokia, e); break;
                case (ENT_EXPLODE): UpdateExplode(nokia, e); break;
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
    if (gScore > 999999) gScore = 999999;
    nkSetText(nokia, 3,0, NK_FALSE, "%06d", gScore);
}
