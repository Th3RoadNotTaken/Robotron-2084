#pragma once
#include "baseTypes.h"
#include "sprite.h"
#include "Object.h"
#include "Robotron.h"

// Animation constants
#define ENFORCER_FLY_ANIMS               4

#define ENFORCER_FRAME_DUR               100

#define ENFORCER_FLY_1_POS_X             0
#define ENFORCER_FLY_2_POS_X             9
#define ENFORCER_FLY_3_POS_X             16
#define ENFORCER_FLY_4_POS_X             25
#define ENFORCER_FLY_POS_Y               0

#define ENFORCER_FLY_SIZE_X              7
#define ENFORCER_FLY_SIZE_Y              7

#define ENFORCER_BOUND_X	             7
#define ENFORCER_BOUND_Y	             7

#define ENFORCER_SPRITE_SCALE            3.0f

// Object Properties
#define ENFORCER_MIN_VELOCITY		     0.3f
#define ENFORCER_MAX_VELOCITY		     0.5f
#define ENFORCER_CHAR_DIAGONAL_SPEED     0.7071f	// 1/sqrt(2) for normalizing magnitude

typedef struct Enforcer_t
{
    Object Obj;
    Bounds2D Bounds;
    Coord2D TargetPoint;
    bool VelocityInitialized;
    SpriteAnimation* FlyAnim;
    Coord2D SheetSize;
    Robotron* Robotron_Obj;
} Enforcer;

typedef void (*EnforcerBoundaryCollide)(Object*);
void EnforcerSetCollideCB(EnforcerBoundaryCollide cb);
void EnforcerClearCollideCB();
void EnforcerCheckBoundaryColl(Enforcer* Enforcer_Obj);

void EnforcerInitTextures();
void EnforcerInitAnimations(Enforcer* Enforcer_Obj);
Bounds2D CalculateEnforcerBounds(Coord2D Position);
Enforcer* EnforcerNew(Coord2D Position, Bounds2D FieldBounds, Robotron* Robotron_Obj, bool IsActive);
void EnforcerDelete(Enforcer* Enforcer_Obj);