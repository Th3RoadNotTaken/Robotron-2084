#pragma once
#include "baseTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_RECTS                       60
#define RECT_BATCHES                    3
#define RECTS_PER_BATCH                 (MAX_RECTS/RECT_BATCHES)
#define MAX_RECT_WIDTH                  1400
#define BASE_WIDTH                      10.0f
#define BASE_HEIGHT                     4.0f

#define NO_CENTER_SPAWN_THRESHOLD       50.0f
#define NO_OUTSKIRTS_SPAWN_THRESHOLD    20.0f

typedef struct leveldef_t {
    uint8_t levelNumber;
    Coord2D MenuPos;
    Coord2D StartPos;
    Coord2D GameOverScreenPos;
    Bounds2D fieldBounds;
    Bounds2D spawnOuterBounds;
    Bounds2D spawnInnerBounds;
    Bounds2D otherTypespawnOuterBounds;
    Bounds2D otherTypespawnInnerBounds;
    uint32_t fieldColor;
    Coord2D ScorePos;
    Coord2D FinalScorePos;
    Coord2D HighScoresPos;
    Coord2D LivesPos;
    Coord2D WaveTextPos;
    Coord2D WaveNumPos;
    uint32_t GruntScore;
    uint32_t SpheroidScore;
    uint32_t EnforcerScore;
    uint32_t QuarkScore;
    uint32_t TankScore;
    uint32_t SOBSScore;
    uint32_t COBSScore;
    uint32_t StaticObsScore;
    uint32_t StaticObs2084Score;
    uint32_t minGrunts;
    uint32_t maxGrunts;
    uint32_t minGruntScale;
    uint32_t maxGruntScale;
    uint32_t minHulks;
    uint32_t maxHulks;
    uint32_t minHulkScale;
    uint32_t maxHulkScale;
    uint32_t minSpheroids;
    uint32_t maxSpheroids;
    uint32_t minSpheroidScale;
    uint32_t maxSpheroidScale;
    uint32_t minQuarks;
    uint32_t maxQuarks;
    uint32_t minQuarkScale;
    uint32_t maxQuarkScale;
    uint32_t minSOBS;
    uint32_t maxSOBS;
    uint32_t minSOBSScale;
    uint32_t maxSOBSScale;
    uint32_t minCOBS;
    uint32_t maxCOBS;
    uint32_t minCOBSScale;
    uint32_t maxCOBSScale;
    uint32_t minStaticObs;
    uint32_t maxStaticObs;
    uint32_t minStaticObsScale;
    uint32_t maxStaticObsScale;
    uint32_t minStatic2084Obs;
    uint32_t maxStatic2084Obs;
    uint32_t minStatic2084ObsScale;
    uint32_t maxStatic2084ObsScale;
} LevelDef;

typedef struct level_t Level;

typedef void (*UpdateLevel)(Level*);
void levelMgrSetUpdLevelCB(UpdateLevel cb);
void levelMgrClearUpdLevelCB();

static uint32_t waveNum = 0;

void levelMgrInit(const LevelDef* leveldef);
void levelMgrShutdown();
Level *levelMgrLoad(uint8_t levelNumber);
void levelMgrUpdate(uint32_t milliseconds);
void levelMgrDraw();
void PlayLevelTransitionAnim(uint32_t milliseconds);
void ResetLevelTransitionProps();
void DrawTransition();
uint32_t rectGetRandomColor();
void levelReset();
void levelNext();
Coord2D getRandSpawnPoint(Bounds2D OuterBounds, Bounds2D InnerBounds);
Coord2D getRandSpawnPointInField();
void setTimer(float Duration);
void ResetScore();
void CalculateHighScoreVals(const char* FileName);
void levelMgrUnload(Level* level, uint8_t levelNumber);
void levelMgrUnloadAll(Level* level);

#ifdef __cplusplus
}
#endif