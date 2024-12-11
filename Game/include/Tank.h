#pragma once
#include "baseTypes.h"
#include "sprite.h"
#include "Object.h"
#include "Robotron.h"

// Animation constants
#define TANK_FLY_POS_X				 0
#define TANK_FLY_POS_Y				 0
#define TANK_FLY_SIZE_X				 7
#define TANK_FLY_SIZE_Y				 7

#define TANK_BOUND_X				 7
#define TANK_BOUND_Y				 7

#define TANK_SPRITE_SCALE			 3.2f

// Object Properties
#define TANK_MIN_VELOCITY		     0.25f	
#define TANK_MAX_VELOCITY		     0.4f
#define TANK_CHAR_DIAGONAL_SPEED     0.7071f	// 1/sqrt(2) for normalizing magnitude

typedef struct Tank_t
{
    Object Obj;
    Bounds2D Bounds;
    Coord2D TargetPoint;
    bool VelocityInitialized;
    SpriteFrame FlyFrame;
    Coord2D SheetSize;
    Robotron* Robotron_Obj;
} Tank;

void TankInitTextures();
void TankInitAnimations(Tank* Tank_Obj);
Bounds2D CalculateTankBounds(Coord2D Position);
Tank* TankNew(Coord2D Position, Bounds2D FieldBounds, Robotron* Robotron_Obj, bool IsActive);
void TankDelete(Tank* Tank_Obj);