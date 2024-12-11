#pragma once
#include "baseTypes.h"
#include "Object.h"
#include "sprite.h"

// Score UI properties
#define SCORE_FRAMES              10

#define SCORE_0_POS_X             0
#define SCORE_1_POS_X             10
#define SCORE_2_POS_X             18
#define SCORE_3_POS_X             27
#define SCORE_4_POS_X             36
#define SCORE_5_POS_X             45
#define SCORE_6_POS_X             54
#define SCORE_7_POS_X             63
#define SCORE_8_POS_X             72
#define SCORE_9_POS_X             81

#define SCORE_POS_Y               0
#define SCORE_SIZE_X              3
#define SCORE_SIZE_Y              5

#define SCORE_SPRITE_SCALE        4.0f
#define FINAL_SCORE_SPRITE_SCALE  9.0f
#define HIGHSCORES_GAP            80 
#define NUMBER_GAP                15
#define FINAL_SCORE_NUMBER_GAP    30

// Lives UI properties
#define LIVES_POS_X               0
#define LIVES_POS_Y               0
#define LIVES_SIZE_X              7
#define LIVES_SIZE_Y              11

#define LIVES_SPRITE_SCALE        2.4
#define LIVES_GAP                 22

// Wave text UI properties
#define WAVE_POS_X                0
#define WAVE_POS_Y                0
#define WAVE_SIZE_X               23
#define WAVE_SIZE_Y               6

#define WAVE_SPRITE_SCALE         2.5

typedef struct UI_t
{
    Object Obj;
    uint8_t LevelNumber;
    Bounds2D Bounds;
    uint32_t ScoreVal;
    uint32_t HighScoreVals[3];
    Coord2D ScorePos;
    Coord2D HighScoresPos;
    float ScoreSpriteScale;
    int NumberGap;
    Coord2D NumberSheetSize;
    SpriteFrame NumberFrames[SCORE_FRAMES];
    uint8_t LivesVal;
    Coord2D LivesPos;
    Coord2D LivesSheetSize;
    SpriteFrame LivesFrame;
    uint32_t WaveNum;
    Coord2D WaveNumPos;
    SpriteFrame WaveTextFrame;
    Coord2D WaveTextSheetSize;
    Coord2D WaveTextPos;
} UIElements;

void UIInitTextures();
void ScoreInitFrames(UIElements* UI_Obj);
UIElements* UINew(Bounds2D FieldBounds, uint8_t LevelNumber, bool IsActive, Coord2D ScorePos, Coord2D HighScoresPos, uint8_t LivesVal, Coord2D LivesPos, Coord2D WaveTextPos, uint32_t WaveNum, Coord2D WaveNumPos);
void UIDelete(UIElements* UI_Obj);