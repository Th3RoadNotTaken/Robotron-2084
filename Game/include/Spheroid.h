#pragma once
#include "baseTypes.h"
#include "sprite.h"
#include "Object.h"
#include "Robotron.h"
#include "ProjectileManager.h"

// Animation constants
#define SPHEROID_SPAWN_ANIMS                6

#define SPHEROID_FRAME_DUR                  100

#define SPHEROID_SPAWN_1_POS_X              0
#define SPHEROID_SPAWN_1_POS_Y              4
#define SPHEROID_SPAWN_1_SIZE_X             3
#define SPHEROID_SPAWN_1_SIZE_Y             4

#define SPHEROID_SPAWN_2_POS_X              7
#define SPHEROID_SPAWN_2_POS_Y              3
#define SPHEROID_SPAWN_2_SIZE_X             3
#define SPHEROID_SPAWN_2_SIZE_Y             5

#define SPHEROID_SPAWN_3_POS_X              14
#define SPHEROID_SPAWN_3_POS_Y              3
#define SPHEROID_SPAWN_3_SIZE_X             4
#define SPHEROID_SPAWN_3_SIZE_Y             6

#define SPHEROID_SPAWN_4_POS_X              23
#define SPHEROID_SPAWN_4_POS_Y              1
#define SPHEROID_SPAWN_4_SIZE_X             5
#define SPHEROID_SPAWN_4_SIZE_Y             9

#define SPHEROID_SPAWN_5_POS_X              33
#define SPHEROID_SPAWN_5_POS_Y              1
#define SPHEROID_SPAWN_5_SIZE_X             6
#define SPHEROID_SPAWN_5_SIZE_Y             9

#define SPHEROID_SPAWN_6_POS_X              44
#define SPHEROID_SPAWN_6_POS_Y              0
#define SPHEROID_SPAWN_6_SIZE_X             7
#define SPHEROID_SPAWN_6_SIZE_Y             11

#define SPHEROID_BOUND_X	                7
#define SPHEROID_BOUND_Y	                11

#define SPHEROID_SPRITE_SCALE               2.8f

// Object Properties
#define SPHEROID_MIN_VELOCITY		        0.15f
#define SPHEROID_MAX_VELOCITY		        0.3f
#define SPHEROID_CHAR_DIAGONAL_SPEED        0.7071f	// 1/sqrt(2) for normalizing magnitude

#define SPHEROID_MIN_FLY_DURATION           2000.0f
#define SPHEROID_MAX_FLY_DURATION           4000.0f

#define SPHEROID_MIN_ENFORCER_SPAWN_DELAY   3000.0f
#define SPHEROID_MAX_ENFORCER_SPAWN_DELAY   5000.0f

typedef struct Spheroid_t
{
    Object Obj;
    Bounds2D Bounds;
    float FlyDuration;
    float FlyElapsedTime;
    Coord2D NextWaypoint;
    SpriteAnimation* SpawnAnim;
    bool PlaySpawnAnim;
    Coord2D SheetSize;
    Robotron* Robotron_Obj;
    ProjectileManager* Manager;
    float EnforcerSpawnElapsedTime;
    float EnforcerSpawnDelay;
} Spheroid;

void SpheroidInitTextures();
void SpheroidInitAnimations(Spheroid* Spheroid_Obj);
void SpheroidPlaySpawnAnimation(Spheroid* Spheroid_Obj, uint32_t milliseconds);
Bounds2D CalculateSpheroidBounds(Coord2D Position);
Spheroid* SpheroidNew(Coord2D Position, Bounds2D FieldBounds, Robotron* Robotron_Obj, bool IsActive, ProjectileManager* Manager);
void SpheroidDelete(Spheroid* Spheroid_Obj);