#pragma once
#include "baseTypes.h"
#include "sprite.h"
#include "Object.h"
#include "Robotron.h"

// Animation constants
#define GRUNT_RUNANIM_TYPES         3

#define GRUNT_RUN_POS_Y             0
#define GRUNT_FRAME_DUR             200

#define GRUNT_RUN_1_POS_X           0
#define GRUNT_RUN_2_POS_X           15
#define GRUNT_RUN_3_POS_X           30
#define GRUNT_RUN_SIZE_X            9
#define GRUNT_RUN_SIZE_Y            13

#define GRUNT_BOUND_X	            9
#define GRUNT_BOUND_Y	            13

#define GRUNT_SPRITE_SCALE          3.0f

// Object Properties
#define GRUNT_MIN_VELOCITY		    0.1f
#define GRUNT_MAX_VELOCITY		    0.15f
#define GRUNT_CHAR_DIAGONAL_SPEED   0.7071f	// 1/sqrt(2) for normalizing magnitude

#define DELTA_MOVEMENT              0.25f      
#define WAVE_AMPLITUDE              0.8f   // The height of the wave oscillation
#define WAVE_SPEED                  0.005f // Controls how fast the wave oscillates

#define GRUNT_MOVE_MIN_DURATION     300.0f
#define GRUNT_MOVE_MAX_DURATION     500.0f

#define GRUNT_STOP_MIN_DURATION     300.0f
#define GRUNT_STOP_MAX_DURATION     400.0f

typedef struct Grunt_t
{
    Object Obj;
    Bounds2D Bounds;
    SpriteAnimation* RunAnim;
    Coord2D SheetSize;
    Robotron* Robotron_Obj;
    float MoveElapsedTime;
    float MoveDuration;
    float StopElapsedTime;
    float StopDuration;
} Grunt;

void GruntInitTextures();
void GruntInitAnimations(Grunt* Grunt_Obj);
Bounds2D CalculateGruntBounds(Coord2D Position);
Grunt* GruntNew(Coord2D Position, Bounds2D FieldBounds, Robotron* Robotron_Obj, bool IsActive);
void GruntDelete(Grunt* Grunt_Obj);