#pragma once
#include "baseTypes.h"
#include "sprite.h"
#include "Object.h"

// Obstacle frames
#define OBS_NUM                 7

#define OBS_1_POS_X				0
#define OBS_1_POS_Y				1
#define OBS_1_SIZE_X			9
#define OBS_1_SIZE_Y			9

#define OBS_2_POS_X				10
#define OBS_2_POS_Y				1
#define OBS_2_SIZE_X			9
#define OBS_2_SIZE_Y			9

#define OBS_3_POS_X				20
#define OBS_3_POS_Y				1
#define OBS_3_SIZE_X			9
#define OBS_3_SIZE_Y			9

#define OBS_4_POS_X				30
#define OBS_4_POS_Y				1
#define OBS_4_SIZE_X			9
#define OBS_4_SIZE_Y			9

#define OBS_5_POS_X				46
#define OBS_5_POS_Y				1
#define OBS_5_SIZE_X			9
#define OBS_5_SIZE_Y			9

#define OBS_6_POS_X				74
#define OBS_6_POS_Y				1
#define OBS_6_SIZE_X			9
#define OBS_6_SIZE_Y			9

#define OBS_7_POS_X				84
#define OBS_7_POS_Y				0
#define OBS_7_SIZE_X			9
#define OBS_7_SIZE_Y			10

#define OBS_BOUND_X			    9
#define OBS_BOUND_Y			    9

#define OBS_SPRITE_SCALE		3.3f

#define OBS_2084_POS_X			56
#define OBS_2084_POS_Y			2
#define OBS_2084_SIZE_X			17
#define OBS_2084_SIZE_Y			7

#define OBS_2084_SPRITE_SCALE	3.3f

typedef struct OBS_t
{
    Object Obj;
    Bounds2D Bounds;
    SpriteFrame* OBS_SpriteFrames;
    uint8_t CurrentFrame;
    Coord2D SheetSize;
} OBS;

typedef struct OBS_2084_t
{
    Object Obj;
    Bounds2D Bounds;
    SpriteFrame OBS_2084_SpriteFrame;
    Coord2D SheetSize;
} OBS_2084;

void OBSInitTextures();
void OBSInitSprites(OBS* OBS_Obj);
void OBS_2084_InitSprites(OBS_2084* OBS_2084_Obj);
Bounds2D CalculateOBSBounds(Coord2D Position);
Bounds2D CalculateOBS_2084_Bounds(Coord2D Position);
OBS* OBSNew(Coord2D Position, Bounds2D FieldBounds, bool IsActive);
OBS_2084* OBS_2084_New(Coord2D Position, Bounds2D FieldBounds, bool IsActive);
void OBSDelete(OBS* OBS_Obj);
void OBS_2084_Delete(OBS_2084* OBS_2084_Obj);