#pragma once
#include "baseTypes.h"
#include "sprite.h"
#include "Object.h"
#include "Robotron.h"

// Animation constants
#define HULK_RUNANIM_TYPES        3

#define HULK_FRAME_DUR            200

#define HULK_RL_1_POS_X           0
#define HULK_RL_1_POS_Y           1
#define HULK_RL_2_POS_X           16
#define HULK_RL_2_POS_Y           1
#define HULK_RL_3_POS_X           34
#define HULK_RL_3_POS_Y           0

#define HULK_RL_SIZE_X            12
#define HULK_RL_SIZE_Y            14

#define HULK_RD_1_POS_X           53
#define HULK_RD_1_POS_Y           0
#define HULK_RD_2_POS_X           72
#define HULK_RD_2_POS_Y           0
#define HULK_RD_3_POS_X           91
#define HULK_RD_3_POS_Y           0

#define HULK_RD_SIZE_X            13
#define HULK_RD_SIZE_Y            16

#define HULK_RR_1_POS_X           112
#define HULK_RR_1_POS_Y           0
#define HULK_RR_2_POS_X           129
#define HULK_RR_2_POS_Y           0
#define HULK_RR_3_POS_X           148
#define HULK_RR_3_POS_Y           0

#define HULK_RR_SIZE_X            12
#define HULK_RR_SIZE_Y            14

#define HULK_BOUND_X	          13
#define HULK_BOUND_Y	          16

#define HULK_SPRITE_SCALE          2.6f

// Object Properties
#define HULK_MIN_VELOCITY		   0.02f
#define HULK_MAX_VELOCITY		   0.05f
#define HULK_CHAR_DIAGONAL_SPEED   0.7071f	// 1/sqrt(2) for normalizing magnitude

#define HULK_MIN_WALK_DURATION     4000.0f
#define HULK_MAX_WALK_DURATION     6000.0f

typedef struct Hulk_t
{
    Object Obj;
    Bounds2D Bounds;
    float WalkDuration;
    float WalkElapsedTime;
    SpriteAnimation* RunLeftAnim;
    SpriteAnimation* RunDownAnim;
    SpriteAnimation* RunRightAnim;
    Coord2D SheetSize;
    Robotron* Robotron_Obj;
} Hulk;

void HulkInitTextures();
void HulkInitAnimations(Hulk* Hulk_Obj);
Bounds2D CalculateHulkBounds(Coord2D Position);
Hulk* HulkNew(Coord2D Position, Bounds2D FieldBounds, Robotron* Robotron_Obj, bool IsActive);
void HulkDelete(Hulk* Hulk_Obj);