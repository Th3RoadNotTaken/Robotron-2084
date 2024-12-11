#pragma once
#include "baseTypes.h"
#include "Object.h"
#include "sprite.h"

// Explosion animation properties
#define EXP_ANIM_TYPES			9
#define ANIM_FRAME_DUR          50

#define EXP_1_POS_X				6
#define EXP_2_POS_X				68
#define EXP_3_POS_X				135
#define EXP_4_POS_X				192
#define EXP_5_POS_X				262
#define EXP_6_POS_X				326
#define EXP_7_POS_X				388
#define EXP_8_POS_X				452
#define EXP_9_POS_X				510

#define EXP_TYPE1_POS_Y			0
#define EXP_TYPE2_POS_Y			64
#define EXP_TYPE3_POS_Y			128
#define EXP_TYPE4_POS_Y			192
#define EXP_TYPE5_POS_Y			256
#define EXP_TYPE6_POS_Y			320
#define EXP_TYPE7_POS_Y			384
#define EXP_TYPE8_POS_Y			448
#define EXP_TYPE9_POS_Y			512

#define EXP_SIZE_X				63
#define EXP_SIZE_Y				63

#define EXP_SPRITE_SCALE        0.6

typedef struct Explosion_t
{
    Object Obj;
    Bounds2D Bounds;
    Coord2D ExplosionPos;
    Coord2D ExplosionSheetSize;
    bool shouldPlayAnim;
    SpriteAnimation** ExplosionAnimations;
    uint8_t ExplosionAnimIndex;
} Explosion;

// Callback to recycle explosion object when the animation completes
typedef void (*AnimComplete)(Object*);
void AnimCompleteSetCollideCB(AnimComplete cb);
void AnimCompleteClearCollideCB();

void ExplosionInitTextures();
void ExplosionInitAnimations(Explosion* Explosion_Obj);
Explosion* ExplosionNew(Bounds2D FieldBounds, bool IsActive, Coord2D Position);
void ExplosionDelete(Explosion* Explosion_Obj);