#pragma once
#include "baseTypes.h"
#include "sprite.h"
#include "Object.h"

// Animation constants
#define RUNANIM_TYPES         3

#define RUN_POS_Y             0
#define FRAME_DUR             100

#define RU_1_POS_X            116
#define RU_2_POS_X            129
#define RU_3_POS_X            142
#define RU_SIZE_X             7
#define RU_SIZE_Y             12

#define RD_1_POS_X            77
#define RD_2_POS_X            90
#define RD_3_POS_X            103
#define RD_SIZE_X             7
#define RD_SIZE_Y             12

#define RL_1_POS_X            0
#define RL_2_POS_X            13
#define RL_3_POS_X            26
#define RL_SIZE_X             5
#define RL_SIZE_Y             11

#define RR_1_POS_X            38
#define RR_2_POS_X            51
#define RR_3_POS_X            64
#define RR_SIZE_X             5
#define RR_SIZE_Y             11

#define BOUND_X	              7
#define BOUND_Y	              12

#define SPRITE_SCALE          4.0f

// Object Properties
#define DEF_VELOCITY		  0.3f
#define CHAR_DIAGONAL_SPEED   0.7071f	// 1/sqrt(2) for normalizing magnitude
#define DEF_LIVES             2
#define MAX_LIVES             3

typedef struct Robotron_t
{
    Object Obj;
    Bounds2D Bounds;
    SpriteAnimation* RunUpAnim;
    SpriteAnimation* RunDownAnim;
    SpriteAnimation* RunLeftAnim;
    SpriteAnimation* RunRightAnim;
    Coord2D SheetSize;
    uint8_t NumLives;
    uint8_t MaxLives;
} Robotron;

typedef void (*RobtronCollideCB)(Robotron*);
void RobotronSetCollideCB(RobtronCollideCB cb, bool IsDead);
void RobotronClearCollideCB(bool IsDead);

enum Direction AimDirection;

void RobotronInitTextures();
void RobotronInitAnimations(Robotron* Robotron_Obj);
Bounds2D CalculateRobotronBounds(Coord2D Position);
Robotron* RobotronNew(Coord2D Position, Bounds2D FieldBounds);
void RobotronDelete(Robotron* Robotron_Obj);
void HandleMovementInput(Object* obj, uint32_t milliseconds);
void HandleAimingInput(Object* obj, uint32_t milliseconds);
void RobotronRecycleAllBullets();

uint8_t GetCurrentLives(Robotron* Robotron_Obj);
void ResetLives(Robotron* Robotron_Obj);
void ReduceLives(Robotron* Robotron_Obj);
void IncreaseLives(Robotron* Robotron_Obj);