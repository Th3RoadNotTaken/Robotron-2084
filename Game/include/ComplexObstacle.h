#pragma once
#include "baseTypes.h"
#include "sprite.h"
#include "Object.h"

// Animation constants
#define COBS_MOVE_ANIMS                 8

#define COBS_FRAME_DUR                  60

#define COBS_MOVE_1_POS_X               0
#define COBS_MOVE_1_POS_Y               6
#define COBS_MOVE_1_SIZE_X              3
#define COBS_MOVE_1_SIZE_Y              3

#define COBS_MOVE_2_POS_X               7
#define COBS_MOVE_2_POS_Y               5
#define COBS_MOVE_2_SIZE_X              5
#define COBS_MOVE_2_SIZE_Y              5

#define COBS_MOVE_3_POS_X               16
#define COBS_MOVE_3_POS_Y               4
#define COBS_MOVE_3_SIZE_X              7
#define COBS_MOVE_3_SIZE_Y              7

#define COBS_MOVE_4_POS_X               27
#define COBS_MOVE_4_POS_Y               3
#define COBS_MOVE_4_SIZE_X              9
#define COBS_MOVE_4_SIZE_Y              9

#define COBS_MOVE_5_POS_X               40
#define COBS_MOVE_5_POS_Y               2
#define COBS_MOVE_5_SIZE_X              11
#define COBS_MOVE_5_SIZE_Y              11

#define COBS_MOVE_6_POS_X               55
#define COBS_MOVE_6_POS_Y               1
#define COBS_MOVE_6_SIZE_X              13
#define COBS_MOVE_6_SIZE_Y              13

#define COBS_MOVE_7_POS_X               72
#define COBS_MOVE_7_POS_Y               0
#define COBS_MOVE_7_SIZE_X              15
#define COBS_MOVE_7_SIZE_Y              15

#define COBS_MOVE_8_POS_X               91
#define COBS_MOVE_8_POS_Y               0
#define COBS_MOVE_8_SIZE_X              15
#define COBS_MOVE_8_SIZE_Y              15

#define COBS_BOUND_X	                15
#define COBS_BOUND_Y	                15

#define COBS_SPRITE_SCALE               2.8f

// Object Properties
#define COBS_MIN_VELOCITY		        0.15f
#define COBS_MAX_VELOCITY		        0.3f
#define COBS_CHAR_DIAGONAL_SPEED        0.7071f	// 1/sqrt(2) for normalizing magnitude

#define COBS_MIN_FLY_DURATION           3000.0f
#define COBS_MAX_FLY_DURATION           4500.0f

// Spawn properties
#define COBS_SPAWN_TIME_MIN_DURATION    500.0f
#define COBS_SPAWN_TIME_MAX_DURATION    1000.0f

typedef struct COBS_t
{
    Object Obj;
    Bounds2D Bounds;
    float FlyDuration;
    float FlyElapsedTime;
    Coord2D NextWaypoint;
    SpriteAnimation* MoveAnim;
    Coord2D SheetSize;
    float ElapsedSpawnTime;
    float SpawnDelay;
} COBS;

void COBSInitTextures();
void COBSInitAnimations(COBS* COBS_Obj);
Bounds2D CalculateCOBSBounds(Coord2D Position);
COBS* COBSNew(Coord2D Position, Bounds2D FieldBounds, bool IsActive);
void COBSDelete(COBS* COBS_Obj);