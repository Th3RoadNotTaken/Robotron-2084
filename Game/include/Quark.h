#pragma once
#include "baseTypes.h"
#include "sprite.h"
#include "Object.h"
#include "Robotron.h"
#include "ProjectileManager.h"

// Animation constants
#define QUARK_FLY_ANIMS					 4

#define QUARK_FRAME_DUR					 100

#define QUARK_FLY_1_POS_X				 0
#define QUARK_FLY_2_POS_X				 15
#define QUARK_FLY_3_POS_X				 30
#define QUARK_FLY_4_POS_X				 46
#define QUARK_FLY_4_SIZE_X				 11
#define QUARK_FLY_4_SIZE_Y				 16

#define QUARK_FLY_POS_Y					 0
#define QUARK_FLY_SIZE_X				 13
#define QUARK_FLY_SIZE_Y				 16

#define QUARK_BOUND_X					 13
#define QUARK_BOUND_Y					 16

#define QUARK_SPRITE_SCALE               2.8f

// Object Properties
#define QUARK_MIN_VELOCITY		         0.2f
#define QUARK_MAX_VELOCITY		         0.35f
#define QUARK_CHAR_DIAGONAL_SPEED        0.7071f	// 1/sqrt(2) for normalizing magnitude

#define QUARK_MIN_FLY_DURATION           2000.0f
#define QUARK_MAX_FLY_DURATION           5000.0f

#define QUARK_MIN_TANK_SPAWN_DELAY		 1500.0f
#define QUARK_MAX_TANK_SPAWN_DELAY       3000.0f

typedef struct Quark_t
{
    Object Obj;
    Bounds2D Bounds;
    float FlyDuration;
    float FlyElapsedTime;
    Coord2D NextWaypoint;
    SpriteAnimation* FlyAnim;
    Coord2D SheetSize;
    Robotron* Robotron_Obj;
    ProjectileManager* Manager;
    float TankSpawnElapsedTime;
    float TankSpawnDelay;
} Quark;

void QuarkInitTextures();
void QuarkInitAnimations(Quark* Quark_Obj);
Bounds2D CalculateQuarkBounds(Coord2D Position);
Quark* QuarkNew(Coord2D Position, Bounds2D FieldBounds, Robotron* Robotron_Obj, bool IsActive, ProjectileManager* Manager);
void QuarkDelete(Quark* Quark_Obj);