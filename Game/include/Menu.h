#pragma once
#include "baseTypes.h"
#include "sprite.h"
#include "Object.h"

// Main Menu properties
#define MM_POS_X             0
#define MM_POS_Y             0
#define MM_SIZE_X            292
#define MM_SIZE_Y            240

#define MM_SCALE			 4

// Start button properties
#define SB_POS_X             0
#define SB_POS_Y             0
#define SB_SIZE_X            77
#define SB_SIZE_Y            16

#define SB_SCALE			 4.5

// Game over properties
#define GO_POS_X             0
#define GO_POS_Y             0
#define GO_SIZE_X            100
#define GO_SIZE_Y            100

#define GO_SCALE_X			 8
#define GO_SCALE_Y			 10

typedef struct Menu_t
{
    Object Obj;
    uint8_t LevelNumber;
    Bounds2D Bounds;
    Coord2D MenuPos;
    Coord2D MenuSheetSize;
    SpriteFrame MenuSpriteFrame;
    Coord2D StartPos;
    Coord2D StartSheetSize;
    SpriteFrame StartSpriteFrame;
    Coord2D GameOverPos;
    Coord2D GameOverSheetSize;
    SpriteFrame GameOverSpriteFrame;
} Menu;

void MenuInitTextures();
void MenuInitFrames(Menu* Menu_Obj);
Menu* MenuNew(Coord2D MainMenuPos, Coord2D StartPos, Coord2D GameOverPos, uint8_t LevelNumber);
void MenuDelete(Menu* Menu_Obj);