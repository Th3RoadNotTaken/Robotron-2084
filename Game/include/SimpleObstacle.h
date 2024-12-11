#pragma once
#include "baseTypes.h"
#include "sprite.h"
#include "Object.h"

// Animation constants
#define SOBS_MOVE_ANIMS                 8

#define SOBS_FRAME_DUR                  60

#define SOBS_MOVE_1_POS_X               0
#define SOBS_MOVE_1_POS_Y               7
#define SOBS_MOVE_1_SIZE_X              1
#define SOBS_MOVE_1_SIZE_Y              1

#define SOBS_MOVE_2_POS_X               5
#define SOBS_MOVE_2_POS_Y               6
#define SOBS_MOVE_2_SIZE_X              3
#define SOBS_MOVE_2_SIZE_Y              3

#define SOBS_MOVE_3_POS_X               12
#define SOBS_MOVE_3_POS_Y               5
#define SOBS_MOVE_3_SIZE_X              5
#define SOBS_MOVE_3_SIZE_Y              5

#define SOBS_MOVE_4_POS_X               21
#define SOBS_MOVE_4_POS_Y               4
#define SOBS_MOVE_4_SIZE_X              7
#define SOBS_MOVE_4_SIZE_Y              7

#define SOBS_MOVE_5_POS_X               32
#define SOBS_MOVE_5_POS_Y               3
#define SOBS_MOVE_5_SIZE_X              9
#define SOBS_MOVE_5_SIZE_Y              9

#define SOBS_MOVE_6_POS_X               45
#define SOBS_MOVE_6_POS_Y               2
#define SOBS_MOVE_6_SIZE_X              11
#define SOBS_MOVE_6_SIZE_Y              11

#define SOBS_MOVE_7_POS_X               60
#define SOBS_MOVE_7_POS_Y               1
#define SOBS_MOVE_7_SIZE_X              13
#define SOBS_MOVE_7_SIZE_Y              13

#define SOBS_MOVE_8_POS_X               77
#define SOBS_MOVE_8_POS_Y               0
#define SOBS_MOVE_8_SIZE_X              15
#define SOBS_MOVE_8_SIZE_Y              15

#define SOBS_BOUND_X	                15
#define SOBS_BOUND_Y	                15

#define SOBS_SPRITE_SCALE               2.8f

// Object Properties
#define SOBS_MIN_VELOCITY		        0.15f
#define SOBS_MAX_VELOCITY		        0.3f
#define SOBS_CHAR_DIAGONAL_SPEED        0.7071f	// 1/sqrt(2) for normalizing magnitude

// Spawn properties
#define SOBS_SPAWN_TIME_MIN_DURATION    500.0f
#define SOBS_SPAWN_TIME_MAX_DURATION    1000.0f

typedef struct SOBS_t
{
    Object Obj;
    Bounds2D Bounds;
    Coord2D NextWaypoint;
    bool VelocityInitialized;
    SpriteAnimation* MoveAnim;
    Coord2D SheetSize;
    float ElapsedSpawnTime;
    float SpawnDelay;
} SOBS;

void SOBSInitTextures();
void SOBSInitAnimations(SOBS* SOBS_Obj);
Bounds2D CalculateSOBSBounds(Coord2D Position);
SOBS* SOBSNew(Coord2D Position, Bounds2D FieldBounds, bool IsActive);
void SOBSDelete(SOBS* SOBS_Obj);