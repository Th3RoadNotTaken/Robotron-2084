#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>   
#include <gl/GLU.h>
#include <assert.h>
#include "baseTypes.h"
#include "levelmgr.h"
#include "field.h"
#include "objmgr.h"
#include "SOIL.h"
#include "sound.h"
#include "shape.h"
#include "Robotron.h"
#include "Bullet.h"
#include "Grunt.h"
#include "Hulk.h"
#include "Spheroid.h"
#include "Enforcer.h"
#include "Explosion.h"
#include "SimpleObstacle.h"
#include "ComplexObstacle.h"
#include "EnemyManager.h"
#include "ProjectileManager.h"
#include "ObstacleManager.h"
#include "UI.h"
#include "Menu.h"
#include "random.h"
#include "time.h"
#include "input.h"
#include "math.h"

typedef struct level_t
{
    uint8_t currentLevelNum;
    Field* field;
    Menu* MainMenu;
    EnemyManager* EnemyPoolManager;
    ProjectileManager* ProjectilePoolManager;
    ObstacleManager* ObstaclePoolManager;
    UIElements* UI;
    uint32_t TotalScore;
    uint32_t ScoreToLifeIncrement;
    uint32_t LifeIncrementScore;
    uint32_t GruntScore;
    uint32_t SpheroidScore;
    uint32_t EnforcerScore;
    uint32_t QuarkScore;
    uint32_t TankScore;
    uint32_t SOBSScore;
    uint32_t COBSScore;
    uint32_t StaticObsScore;
    uint32_t StaticObs2084Score;
    uint8_t RemainingGrunts;
    uint8_t RemainingHulks;
    uint8_t RemainingSpheroids;
    uint8_t RemainingQuarks;
    uint8_t RemainingSOBS;
    uint8_t RemainingCOBS;
    uint8_t RemainingStaticObs;
    uint8_t RemainingStaticObs2084;
    Robotron* Robotron_Obj;
} Level;

static Level* level;
const LevelDef* gameLevelDef;
static uint32_t FinalScore;
static UpdateLevel updLevel = NULL;

// Variables that keep track of wave spawn numbers for particular enemy types
static uint32_t hulkWaveNum = 0;
static uint32_t spheroidWaveNum = 0;
static uint32_t quarkWaveNum = 0;
static uint32_t sobsWaveNum = 0;
static uint32_t cobsWaveNum = 0;
static uint32_t obsWaveNum = 0;
static uint32_t obs2084WaveNum = 0;

// Probability of spawning between 0-1
static float hulkSpawnProb = 0.6f;
static float spheroidSpawnProb = 0.72f;
static float quarkSpawnProb = 0.65f;
static float sobsSpawnProb = 0.8f;
static float cobsSpawnProb = 0.72f;
static float obsSpawnProb = 0.7f;
static float staticObsSpawnProb = 0.7f; // Remaining for static 2084 obstacles

// State reset timer
static bool timerActive = false;
static float elapsedTime = 0.0f;
static float timerDuration = 3000.0f;
static bool clearScreenState = false;

// New wave timer
static bool wavetimerActive = false;
static float waveelapsedTime = 0.0f;
static float wavetimerDuration = 2500.0f;

// Death timer
static bool deathtimerActive = false;
static float deathelapsedTime = 0.0f;
static float deathtimerDuration = 3000.0f;

// Transition animation variables
static uint8_t currentRectangles = 0;
static uint8_t rectsInBatch = RECTS_PER_BATCH;
static uint8_t colorPallete = 0;
static float rectWidth[MAX_RECTS];
static float rectHeight[MAX_RECTS];
static float rectSpawnElapsedTime = 0.0f;
static float rectSpawnDuration = 10.0f;
const float rectWidthExpansionRate = 1.0f;
const float rectHeightExpansionRate = 0.7f;

static float colorChangeDelay = 100.f;
static uint32_t rectColors[MAX_RECTS] = { 0 };
static float colorUpdateElapsedTimes[MAX_RECTS] = { 0 };
static uint8_t currentPallete = 0;

static int32_t _gruntSoundId = SOUND_NOSOUND;
static int32_t _hulkSoundId = SOUND_NOSOUND;
static int32_t _spheroidSoundId = SOUND_NOSOUND;
static int32_t _quarkSoundId = SOUND_NOSOUND;
static int32_t _deathSoundId = SOUND_NOSOUND;
static int32_t _gameOverSoundId = SOUND_NOSOUND;
static int32_t _levelTransitionSoundId = SOUND_NOSOUND;

static void DisableGrunt(Object* Obj);
static void DisableHulk(Object* Obj);
static void DisableSpheriod(Object* Obj);
static void DisableEnforcer(Object* Obj);
static void DisableObstacle(Object* Obj);
static void DisableQuark(Object* Obj);
static void DisableTank(Object* Obj);
static void RecycleExplosion(Object* Obj);
static void ResetState(Robotron* Robotron_Obj);
static void ResetGame(Robotron* Robotron_Obj);
static void AllEnemiesDead();

void levelMgrSetUpdLevelCB(UpdateLevel cb)
{
    updLevel = cb;
}

void levelMgrClearUpdLevelCB()
{
    updLevel = NULL;
}

/// @brief Initialize the level manager
void levelMgrInit(const LevelDef* leveldef)
{
    srand((unsigned int)time(NULL));

    gameLevelDef = leveldef;

    RobotronInitTextures();

    UIInitTextures();
    MenuInitTextures();


    _gruntSoundId = soundLoad("asset/sounds/Impact_3.wav");
    _hulkSoundId = soundLoad("asset/sounds/Impact_Hulk.wav");
    _spheroidSoundId = soundLoad("asset/sounds/Impact_1.wav");
    _quarkSoundId = soundLoad("asset/sounds/Impact_2.wav");
    _deathSoundId = soundLoad("asset/sounds/Death.wav");
    _gameOverSoundId = soundLoad("asset/sounds/GameOver.wav");
    _levelTransitionSoundId = soundLoad("asset/sounds/LevelTransition.wav");
    RobotronSetCollideCB(ResetState, false);
    RobotronSetCollideCB(ResetGame, true);
    EnemyManagerSetAllEnemiesKilledCB(AllEnemiesDead);
    // Set boundary collision callback for the enforcer
    EnforcerSetCollideCB(DisableEnforcer);
}

/// @brief Shutdown the level manager
void levelMgrShutdown()
{
    RobotronClearCollideCB(true);
    RobotronClearCollideCB(false);
    EnemyManagerClearAllEnemiesKilledCB();
    EnforcerClearCollideCB();
    BulletClearCollideCB(EnemyGrunt);
    BulletClearCollideCB(EnemySpheroid);
    BulletClearCollideCB(EnemyEnforcer);
    BulletClearCollideCB(EnemyHulk);
    BulletClearCollideCB(Obstacle);
    BulletClearCollideCB(EnemyQuark);
    BulletClearCollideCB(EnemyTank);
    AnimCompleteClearCollideCB();
    soundUnload(_gruntSoundId);
    soundUnload(_hulkSoundId);
    soundUnload(_spheroidSoundId);
    soundUnload(_quarkSoundId);
    soundUnload(_deathSoundId);
    soundUnload(_gameOverSoundId);
    soundUnload(_levelTransitionSoundId);
}

/// @brief Loads the level and all required objects/assets
/// @param levelDef 
/// @return 
Level* levelMgrLoad(uint8_t levelNumber)
{
    level = malloc(sizeof(Level));
    if (level != NULL)
    {
        switch (levelNumber)
        {
        case 0:
            level->currentLevelNum = 0;

            level->MainMenu = MenuNew(gameLevelDef->MenuPos, gameLevelDef->StartPos, gameLevelDef->GameOverScreenPos, 0);
            break;
        case 1:
            level->currentLevelNum = 1;

            // the field provides the boundaries of the scene & encloses all game objects
            level->field = fieldNew(gameLevelDef->fieldBounds, rectGetRandomColor());

            Coord2D robotronPosition = boundsGetCenter(&gameLevelDef->fieldBounds);
            level->Robotron_Obj = RobotronNew(robotronPosition, gameLevelDef->fieldBounds);

            level->ProjectilePoolManager = ProjectileManagerInit(gameLevelDef->fieldBounds, level->Robotron_Obj);
            level->EnemyPoolManager = EnemyManagerInit(gameLevelDef->fieldBounds, level->Robotron_Obj, level->ProjectilePoolManager);
            level->ObstaclePoolManager = ObstacleManagerInit(gameLevelDef->fieldBounds);
            level->UI = UINew(gameLevelDef->fieldBounds, 1, true, gameLevelDef->ScorePos, gameLevelDef->HighScoresPos, GetCurrentLives(level->Robotron_Obj), gameLevelDef->LivesPos, gameLevelDef->WaveTextPos, waveNum, gameLevelDef->WaveNumPos);
            BulletSetCollideCB(DisableGrunt, EnemyGrunt);
            BulletSetCollideCB(DisableHulk, EnemyHulk);
            BulletSetCollideCB(DisableSpheriod, EnemySpheroid);
            BulletSetCollideCB(DisableEnforcer, EnemyEnforcer);
            BulletSetCollideCB(DisableObstacle, Obstacle);
            BulletSetCollideCB(DisableQuark, EnemyQuark);
            BulletSetCollideCB(DisableTank, EnemyTank);
            AnimCompleteSetCollideCB(RecycleExplosion);

            // Reset enemy, projectile, obstacles, and score variables
            level->TotalScore = 0;
            level->ScoreToLifeIncrement = 0;
            level->LifeIncrementScore = 10000;
            level->GruntScore = gameLevelDef->GruntScore;
            level->SpheroidScore = gameLevelDef->SpheroidScore;
            level->EnforcerScore = gameLevelDef->EnforcerScore;
            level->QuarkScore = gameLevelDef->QuarkScore;
            level->TankScore = gameLevelDef->TankScore;
            level->SOBSScore = gameLevelDef->SOBSScore;
            level->COBSScore = gameLevelDef->COBSScore;
            level->StaticObsScore = gameLevelDef->StaticObsScore;
            level->StaticObs2084Score = gameLevelDef->StaticObs2084Score;
            level->RemainingGrunts = 0;
            level->RemainingHulks = 0;
            level->RemainingSpheroids = 0;
            level->RemainingQuarks = 0;
            level->RemainingSOBS = 0;
            level->RemainingCOBS = 0;
            level->RemainingStaticObs = 0;
            level->RemainingStaticObs2084 = 0;

            hulkWaveNum = 0;
            spheroidWaveNum = 0;
            quarkWaveNum = 0;
            sobsWaveNum = 0;
            cobsWaveNum = 0;
            obsWaveNum = 0;
            obs2084WaveNum = 0;

            waveNum = 0;
            ResetLives(level->Robotron_Obj);
            ResetScore();
            level->UI->LivesVal = GetCurrentLives(level->Robotron_Obj);
            levelNext();
            break;
        case 2:
            level->currentLevelNum = 2;

            level->MainMenu = MenuNew(gameLevelDef->MenuPos, gameLevelDef->StartPos, gameLevelDef->GameOverScreenPos, 2);
            level->UI = UINew(gameLevelDef->fieldBounds, 2, true, gameLevelDef->FinalScorePos, gameLevelDef->HighScoresPos, 0, gameLevelDef->LivesPos, gameLevelDef->WaveTextPos, waveNum, gameLevelDef->WaveNumPos);
            level->UI->ScoreVal = FinalScore;
            CalculateHighScoreVals("HighScores.txt");

            break;
        default:
            break;
        }
    }
    updLevel(level);
    return level;
}

uint32_t rectGetRandomColor()
{
    uint32_t color;
    color = randGetInt(0, 256);
    color += randGetInt(0, 256) << 8;
    color += randGetInt(0, 256) << 16;

    return color;
}

void levelMgrUpdate(uint32_t milliseconds)
{
    switch (level->currentLevelNum)
    {
    case 0:
        if (inputKeyPressed(VK_SPACE))
        {
            levelMgrUnload(level, 0);
        }
        break;
    case 1:
        if (timerActive)
        {
            elapsedTime += milliseconds;
            if (elapsedTime >= timerDuration)
            {
                elapsedTime = 0.0f;
                timerActive = false;

                if (clearScreenState == true)
                {
                    objMgrPauseDrawing();
                    clearScreenState = false;
                    levelReset();

                    // Respawn player at the center
                    Coord2D robotronPosition = boundsGetCenter(&gameLevelDef->fieldBounds);
                    level->Robotron_Obj->Obj.position = robotronPosition;
                }
            }
        }
        else if (wavetimerActive)
        {
            PlayLevelTransitionAnim(milliseconds);
            waveelapsedTime += milliseconds;
            if (waveelapsedTime >= wavetimerDuration)
            {
                waveelapsedTime = 0.0f;
                wavetimerActive = false;

                levelNext();
            }
        }
        else if (deathtimerActive)
        {
            deathelapsedTime += milliseconds;
            if (deathelapsedTime >= deathtimerDuration)
            {
                deathelapsedTime = 0.0f;
                deathtimerActive = false;

                levelMgrUnload(level, level->currentLevelNum);
            }
        }
        break;
    case 2:
        if (inputKeyPressed(0x52))
        {
            levelMgrUnload(level, level->currentLevelNum);
        }
        break;
    }
}

void levelMgrDraw()
{
    DrawTransition();
}

// Plays the level transition animation while wavetimerActive is active
void PlayLevelTransitionAnim(uint32_t milliseconds)
{
    if (rectSpawnElapsedTime >= rectSpawnDuration)
    {
        rectSpawnElapsedTime = 0.0f;
        if (currentRectangles < MAX_RECTS)
            ++currentRectangles;
        else
            return;
        rectColors[currentRectangles - 1] = rectGetRandomColor();
        colorUpdateElapsedTimes[currentRectangles - 1] = 0.0f;

        rectWidth[currentRectangles - 1] = BASE_WIDTH;
        rectHeight[currentRectangles - 1] = BASE_HEIGHT;
    }
    else
    {
        rectSpawnElapsedTime += milliseconds;
        for (int i = 0; i < currentRectangles; i++)
        {
            if (rectWidth[i] >= MAX_RECT_WIDTH)
                continue;

            rectWidth[i] += rectWidthExpansionRate * milliseconds;
            rectHeight[i] += rectHeightExpansionRate * milliseconds;

            colorUpdateElapsedTimes[i] += milliseconds;
            if (colorUpdateElapsedTimes[i] >= colorChangeDelay)
            {
                colorUpdateElapsedTimes[i] = 0.0f;
                rectColors[i] = rectGetRandomColor();
            }
        }
    }

    if (currentRectangles >= rectsInBatch)
    {
        colorPallete = (colorPallete + 1) % 3;
        rectsInBatch += RECTS_PER_BATCH;
    }
}

// Reset all level transition animation properties
void ResetLevelTransitionProps()
{
    for (int i = 0; i < MAX_RECTS; i++)
    {
        rectWidth[i] = BASE_WIDTH;
        rectHeight[i] = BASE_HEIGHT;
        colorUpdateElapsedTimes[i] = 0.0f;
    }
    currentRectangles = 0;
    rectSpawnElapsedTime = 0.0f;
    rectsInBatch = RECTS_PER_BATCH;
    colorPallete = 0;
}

void DrawTransition()
{
    Coord2D fieldCenter = boundsGetCenter(&gameLevelDef->fieldBounds);

    for (int i = 0; i < currentRectangles; i++)
    {
        uint32_t r = ((rectColors[i] >> 16) & 0xFF);
        uint32_t g = ((rectColors[i] >> 8) & 0xFF);
        uint32_t b = ((rectColors[i] >> 0) & 0xFF);

        switch (colorPallete)
        {
        case 0:
            r = 0;
            break;
        case 1:
            g = 0;
            break;
        case 2:
            b = 0;
            break;
        }

        shapeDrawRectangle(fieldCenter.x - rectWidth[i] / 2, fieldCenter.y - rectHeight[i] / 2, fieldCenter.x + rectWidth[i] / 2, fieldCenter.y + rectHeight[i] / 2, r, g, b, 50);
    }
}

void levelReset()
{
    Coord2D* randSpawnPoints = malloc(sizeof(Coord2D) * level->RemainingGrunts);
    assert(randSpawnPoints != NULL);

    // Get rid of any remaining enforcers
    ProjectileManagerRecycleAllEnforcers(level->ProjectilePoolManager);

    // Get rid of any remaining tanks
    ProjectileManagerRecycleAllTanks(level->ProjectilePoolManager);

    // Get rid of any remaining explosions
    ProjectileManagerRecycleAllExplosions(level->ProjectilePoolManager);

    // Respawn grunts
    for (int32_t i = 0; i < level->RemainingGrunts; i++)
    {
        // Get random spawn point
        randSpawnPoints[i] = getRandSpawnPoint(gameLevelDef->spawnOuterBounds, gameLevelDef->spawnInnerBounds);
    }
    EnemyManagerRespawnGrunts(level->EnemyPoolManager, randSpawnPoints);

    free(randSpawnPoints);
    randSpawnPoints = malloc(sizeof(Coord2D) * level->RemainingHulks);
    assert(randSpawnPoints != NULL);

    // Respawn hulks
    for (int32_t i = 0; i < level->RemainingHulks; i++)
    {
        // Get random spawn point
        randSpawnPoints[i] = getRandSpawnPoint(gameLevelDef->spawnOuterBounds, gameLevelDef->otherTypespawnInnerBounds);
    }
    EnemyManagerRespawnHulks(level->EnemyPoolManager, randSpawnPoints);

    free(randSpawnPoints);
    randSpawnPoints = malloc(sizeof(Coord2D) * level->RemainingSpheroids);
    assert(randSpawnPoints != NULL);

    // Respawn spheroids
    for (int32_t i = 0; i < level->RemainingSpheroids; i++)
    {
        // Get random spawn point
        randSpawnPoints[i] = getRandSpawnPoint(gameLevelDef->spawnOuterBounds, gameLevelDef->spawnInnerBounds);
    }
    EnemyManagerRespawnSpheroids(level->EnemyPoolManager, randSpawnPoints);

    free(randSpawnPoints);
    randSpawnPoints = malloc(sizeof(Coord2D) * level->RemainingQuarks);
    assert(randSpawnPoints != NULL);

    // Respawn quarks
    for (int32_t i = 0; i < level->RemainingQuarks; i++)
    {
        // Get random spawn point
        randSpawnPoints[i] = getRandSpawnPoint(gameLevelDef->spawnOuterBounds, gameLevelDef->spawnInnerBounds);
    }
    EnemyManagerRespawnQuarks(level->EnemyPoolManager, randSpawnPoints);

    free(randSpawnPoints);
    randSpawnPoints = malloc(sizeof(Coord2D) * level->RemainingSOBS);
    assert(randSpawnPoints != NULL);

    // Respawn simple obstacles
    for (int32_t i = 0; i < level->RemainingSOBS; i++)
    {
        // Get random spawn point
        randSpawnPoints[i] = getRandSpawnPoint(gameLevelDef->spawnOuterBounds, gameLevelDef->spawnInnerBounds);
    }
    ProjectileManagerRespawnSOBS(level->ProjectilePoolManager, randSpawnPoints);

    free(randSpawnPoints);
    randSpawnPoints = malloc(sizeof(Coord2D) * level->RemainingCOBS);
    assert(randSpawnPoints != NULL);

    // Respawn complex obstacles
    for (int32_t i = 0; i < level->RemainingCOBS; i++)
    {
        // Get random spawn point
        randSpawnPoints[i] = getRandSpawnPoint(gameLevelDef->spawnOuterBounds, gameLevelDef->spawnInnerBounds);
    }
    ProjectileManagerRespawnCOBS(level->ProjectilePoolManager, randSpawnPoints);

    free(randSpawnPoints);
    randSpawnPoints = malloc(sizeof(Coord2D) * level->RemainingStaticObs);
    assert(randSpawnPoints != NULL);

    // Respawn static obstacles
    for (int32_t i = 0; i < level->RemainingStaticObs; i++)
    {
        // Get random spawn point
        randSpawnPoints[i] = getRandSpawnPointInField();
    }
    ObstacleManagerRespawnOBS(level->ObstaclePoolManager, randSpawnPoints);

    free(randSpawnPoints);
    randSpawnPoints = malloc(sizeof(Coord2D) * level->RemainingStaticObs2084);
    assert(randSpawnPoints != NULL);

    // Respawn static 2084 obstacles
    for (int32_t i = 0; i < level->RemainingStaticObs2084; i++)
    {
        // Get random spawn point
        randSpawnPoints[i] = getRandSpawnPointInField();
    }
    ObstacleManagerRespawn_2084_OBS(level->ObstaclePoolManager, randSpawnPoints);

    objMgrResumeDrawing();
    objMgrResume();
}

void levelNext()
{
    ++waveNum;
    level->UI->WaveNum = waveNum;

    float randProbValue;

    //Recycle all objects 
    RecycleAllEnemies(level->EnemyPoolManager);
    RecycleAllProjectiles(level->ProjectilePoolManager);
    RobotronRecycleAllBullets();
    ObstacleManagerRecycleAllOBS(level->ObstaclePoolManager);
    ObstacleManagerRecycleAll_2084_OBS(level->ObstaclePoolManager);

    // Set random field color
    fieldSetColor(level->field, rectGetRandomColor());
    
    // Respawn player in the center
    Coord2D robotronPosition = boundsGetCenter(&gameLevelDef->fieldBounds);
    level->Robotron_Obj->Obj.position = robotronPosition;

    /* Spawn Grunts */
    // Scale minGrunts and maxGrunts based on the wave number
    int32_t scaledMinGrunts = gameLevelDef->minGrunts + waveNum * gameLevelDef->minGruntScale;
    int32_t scaledMaxGrunts = gameLevelDef->maxGrunts + waveNum * gameLevelDef->maxGruntScale;

    // Ensure scaledMinGrunts doesn't exceed scaledMaxGrunts
    if (scaledMinGrunts > scaledMaxGrunts) {
        scaledMinGrunts = scaledMaxGrunts;
    }

    // Get random number of grunts based on scaled values
    int32_t numGrunts = randGetInt(scaledMinGrunts, scaledMaxGrunts);
    level->RemainingGrunts = numGrunts;

    for (int32_t i = 0; i < numGrunts; i++)
    {
        // Get random spawn point
        EnemyManagerSpawnGrunt(level->EnemyPoolManager, getRandSpawnPoint(gameLevelDef->spawnOuterBounds, gameLevelDef->spawnInnerBounds));
    }

    /* Spawn Hulks */
    // Scale minHulks and maxHulks based on the wave number
    randProbValue = randGetFloat(0, 1);
    if (randProbValue <= hulkSpawnProb)
    {
        int32_t scaledMinHulks = gameLevelDef->minHulks + hulkWaveNum * gameLevelDef->minHulkScale;
        int32_t scaledMaxHulks = gameLevelDef->maxHulks + hulkWaveNum * gameLevelDef->maxHulkScale;

        // Ensure scaledMinHulks doesn't exceed scaledMaxHulks
        if (scaledMinHulks > scaledMaxHulks) {
            scaledMinHulks = scaledMaxHulks;
        }

        // Get random number of hulks based on scaled values
        int32_t numHulks = randGetInt(scaledMinHulks, scaledMaxHulks);
        level->RemainingHulks = numHulks;
        ++hulkWaveNum;

        for (int32_t i = 0; i < numHulks; i++)
        {
            // Get random spawn point
            EnemyManagerSpawnHulk(level->EnemyPoolManager, getRandSpawnPoint(gameLevelDef->spawnOuterBounds, gameLevelDef->otherTypespawnInnerBounds));
        }
    }

    /* Spawn Spheroids */
    // Scale minSpheroids and maxSpheroids based on the wave number
    randProbValue = randGetFloat(0, 1);
    if (randProbValue <= spheroidSpawnProb)
    {
        int32_t scaledMinSpheroids = gameLevelDef->minSpheroids + spheroidWaveNum * gameLevelDef->minSpheroidScale;
        int32_t scaledMaxSpheroids = gameLevelDef->maxSpheroids + spheroidWaveNum * gameLevelDef->maxSpheroidScale;

        // Ensure scaledMinSpheroids doesn't exceed scaledMaxSpheroids
        if (scaledMinSpheroids > scaledMaxSpheroids) {
            scaledMinSpheroids = scaledMaxSpheroids;
        }

        // Get random number of spheroids based on scaled values
        int32_t numSpheroids = randGetInt(scaledMinSpheroids, scaledMaxSpheroids);
        level->RemainingSpheroids = numSpheroids;
        ++spheroidWaveNum;

        for (int32_t i = 0; i < numSpheroids; i++)
        {
            // Get random spawn point
            EnemyManagerSpawnSpheroid(level->EnemyPoolManager, getRandSpawnPoint(gameLevelDef->spawnOuterBounds, gameLevelDef->spawnInnerBounds));
        }
    }

    /* Spawn Quarks */
    // Scale minQuarks and maxQuarks based on the wave number
    randProbValue = randGetFloat(0, 1);
    if (randProbValue <= quarkSpawnProb)
    {
        int32_t scaledMinQuarks = gameLevelDef->minQuarks + quarkWaveNum * gameLevelDef->minQuarkScale;
        int32_t scaledMaxQuarks = gameLevelDef->maxQuarks + quarkWaveNum * gameLevelDef->maxQuarkScale;

        // Ensure scaledMinQuarks doesn't exceed scaledMaxQuarks
        if (scaledMinQuarks > scaledMaxQuarks) {
            scaledMinQuarks = scaledMaxQuarks;
        }

        // Get random number of quarks based on scaled values
        int32_t numQuarks = randGetInt(scaledMinQuarks, scaledMaxQuarks);
        level->RemainingQuarks = numQuarks;
        ++quarkWaveNum;

        for (int32_t i = 0; i < numQuarks; i++)
        {
            // Get random spawn point
            EnemyManagerSpawnQuark(level->EnemyPoolManager, getRandSpawnPoint(gameLevelDef->spawnOuterBounds, gameLevelDef->spawnInnerBounds));
        }
    }

    /* Spawn simple obstacles */
    // Scale minSOBS and maxSOBS based on the wave number
    randProbValue = randGetFloat(0, 1);
    if (randProbValue <= sobsSpawnProb)
    {
        int32_t scaledMinSOBS = gameLevelDef->minSOBS + sobsWaveNum * gameLevelDef->minSOBSScale;
        int32_t scaledMaxSOBS = gameLevelDef->maxSOBS + sobsWaveNum * gameLevelDef->maxSOBSScale;

        // Ensure scaledMinSOBS doesn't exceed scaledMaxSOBS
        if (scaledMinSOBS > scaledMaxSOBS) {
            scaledMinSOBS = scaledMaxSOBS;
        }

        // Get random number of simple obstacles based on scaled values
        int32_t numSOBS = randGetInt(scaledMinSOBS, scaledMaxSOBS);
        level->RemainingSOBS = numSOBS;
        ++sobsWaveNum;

        for (int32_t i = 0; i < numSOBS; i++)
        {
            // Get random spawn point
            ProjectileManagerSpawnSOBS(level->ProjectilePoolManager, getRandSpawnPoint(gameLevelDef->spawnOuterBounds, gameLevelDef->spawnInnerBounds));
        }
    }

    /* Spawn complex obstacles */
    // Scale minCOBS and maxCOBS based on the wave number
    randProbValue = randGetFloat(0, 1);
    if (randProbValue <= cobsSpawnProb)
    {
        int32_t scaledMinCOBS = gameLevelDef->minCOBS + cobsWaveNum * gameLevelDef->minCOBSScale;
        int32_t scaledMaxCOBS = gameLevelDef->maxCOBS + cobsWaveNum * gameLevelDef->maxCOBSScale;

        // Ensure scaledMinCOBS doesn't exceed scaledMaxCOBS
        if (scaledMinCOBS > scaledMaxCOBS) {
            scaledMinCOBS = scaledMaxCOBS;
        }

        // Get random number of complex obstacles based on scaled values
        int32_t numCOBS = randGetInt(scaledMinCOBS, scaledMaxCOBS);
        level->RemainingCOBS = numCOBS;
        ++cobsWaveNum;

        for (int32_t i = 0; i < numCOBS; i++)
        {
            // Get random spawn point
            ProjectileManagerSpawnCOBS(level->ProjectilePoolManager, getRandSpawnPoint(gameLevelDef->spawnOuterBounds, gameLevelDef->spawnInnerBounds));
        }
    }

    /* Spawn static obstacles */
    randProbValue = randGetFloat(0, 1);
    if (waveNum >= 3 && randProbValue <= obsSpawnProb)
    {
        // Spawn some type of static obstacle
        randProbValue = randGetFloat(0, 1);
        if (randProbValue <= staticObsSpawnProb)
        {
            // Spawn basic static obstacle
            // Scale minStaticObs and maxStaticObs based on the wave number
            int32_t scaledMinStaticObs = gameLevelDef->minStaticObs + obsWaveNum * gameLevelDef->minStaticObsScale;
            int32_t scaledMaxStaticObs = gameLevelDef->maxStaticObs + obsWaveNum * gameLevelDef->maxStaticObsScale;

            // Ensure scaledMinStaticObs doesn't exceed scaledMaxStaticObs
            if (scaledMinStaticObs > scaledMaxStaticObs) {
                scaledMinStaticObs = scaledMaxStaticObs;
            }

            // Get random number of static obstacles based on scaled values
            int32_t numStaticObs = randGetInt(scaledMinStaticObs, scaledMaxStaticObs);
            level->RemainingStaticObs = numStaticObs;
            ++obsWaveNum;

            for (int32_t i = 0; i < numStaticObs; i++)
            {
                // Get random spawn point
                ObstacleManagerSpawnOBS(level->ObstaclePoolManager, getRandSpawnPointInField());
            }
        }
        else
        {
            // Spawn 2084 static obstacle
            // Scale minStatic2084Obs and maxStatic2084Obs based on the wave number
            int32_t scaledMinStatic2084Obs = gameLevelDef->minStatic2084Obs + obs2084WaveNum * gameLevelDef->minStatic2084ObsScale;
            int32_t scaledMaxStatic2084Obs = gameLevelDef->maxStatic2084Obs + obs2084WaveNum * gameLevelDef->maxStatic2084ObsScale;

            // Ensure scaledMinStatic2084Obs doesn't exceed scaledMaxStatic2084Obs
            if (scaledMinStatic2084Obs > scaledMaxStatic2084Obs) {
                scaledMinStatic2084Obs = scaledMaxStatic2084Obs;
            }

            // Get random number of static 2084 obstacles based on scaled values
            int32_t numStatic2084Obs = randGetInt(scaledMinStatic2084Obs, scaledMaxStatic2084Obs);
            level->RemainingStaticObs2084 = numStatic2084Obs;
            ++obs2084WaveNum;

            for (int32_t i = 0; i < numStatic2084Obs; i++)
            {
                // Get random spawn point
                ObstacleManagerSpawn_2084_OBS(level->ObstaclePoolManager, getRandSpawnPointInField());
            }
        }
    }

    objMgrResumeDrawing();
    objMgrResume();
    ResetLevelTransitionProps();
}

void clearScreen()
{
    setTimer(3000.0f);
    clearScreenState = true;
}

void setTimer(float Duration)
{
    timerDuration = Duration;
    timerActive = true;
}

// Gets a random 2D coordinate between the outer and inner bounds provided
Coord2D getRandSpawnPoint(Bounds2D OuterBounds, Bounds2D InnerBounds)
{
    int32_t spawnSide = randGetInt(0, 4);
    float minX = 0, maxX = 0, minY = 0, maxY = 0;

    switch (spawnSide)
    {
    case 0:
        // Top side
        minX = OuterBounds.topLeft.x;
        maxX = OuterBounds.botRight.x;
        minY = OuterBounds.topLeft.y;
        maxY = InnerBounds.topLeft.y;
        break;
    case 1:
        // Bottom side
        minX = OuterBounds.topLeft.x;
        maxX = OuterBounds.botRight.x;
        minY = InnerBounds.botRight.y;
        maxY = OuterBounds.botRight.y;
        break;
    case 2:
        // Left side
        minX = OuterBounds.topLeft.x;
        maxX = InnerBounds.topLeft.x;
        minY = OuterBounds.topLeft.y;
        maxY = OuterBounds.botRight.y;
        break;
    case 3:
        // Right side
        minX = InnerBounds.botRight.x;
        maxX = OuterBounds.botRight.x;
        minY = OuterBounds.topLeft.y;
        maxY = OuterBounds.botRight.y;
        break;
    }

    float xCoord = randGetFloat(minX, maxX);
    float yCoord = randGetFloat(minY, maxY);

    return (Coord2D) { xCoord, yCoord };
}

// Gets a random 2D coordinate in the entire game field
Coord2D getRandSpawnPointInField()
{
    Coord2D centerPosition = boundsGetCenter(&gameLevelDef->fieldBounds);
    bool FoundPosition = false;
    float randXInBounds, randYInBounds;

    while (FoundPosition != true)
    {
        // Generate random coordinates within the field bounds
        randXInBounds = randGetFloat(gameLevelDef->fieldBounds.topLeft.x, gameLevelDef->fieldBounds.botRight.x);
        randYInBounds = randGetFloat(gameLevelDef->fieldBounds.topLeft.y, gameLevelDef->fieldBounds.botRight.y);

        // Check if the point is sufficiently far from the center
        bool farFromCenter =
            (randXInBounds < centerPosition.x - NO_CENTER_SPAWN_THRESHOLD || randXInBounds > centerPosition.x + NO_CENTER_SPAWN_THRESHOLD) &&
            (randYInBounds < centerPosition.y - NO_CENTER_SPAWN_THRESHOLD || randYInBounds > centerPosition.y + NO_CENTER_SPAWN_THRESHOLD);

        // Check if the point is sufficiently far from all boundaries
        bool farFromBoundaries =
            (randXInBounds > gameLevelDef->fieldBounds.topLeft.x + NO_OUTSKIRTS_SPAWN_THRESHOLD &&
                randXInBounds < gameLevelDef->fieldBounds.botRight.x - NO_OUTSKIRTS_SPAWN_THRESHOLD) &&
            (randYInBounds > gameLevelDef->fieldBounds.topLeft.y + NO_OUTSKIRTS_SPAWN_THRESHOLD &&
                randYInBounds < gameLevelDef->fieldBounds.botRight.y - NO_OUTSKIRTS_SPAWN_THRESHOLD);

        // If both conditions are satisfied, mark the position as valid
        if (farFromCenter && farFromBoundaries)
        {
            FoundPosition = true;
        }
    }
    return (Coord2D) { randXInBounds, randYInBounds };
}

/// @brief Unloads the level and frees up any assets associated
/// @param level 
void levelMgrUnload(Level* level, uint8_t levelNumber)
{
    if (level != NULL)
    {
        switch (levelNumber)
        {
        case 0:
            MenuDelete(level->MainMenu);
            levelMgrLoad(1);
            break;
        case 1:
            FinalScore = level->TotalScore;
            RobotronDelete(level->Robotron_Obj);
            EnemyManagerDestroy(level->EnemyPoolManager);
            ProjectileManagerDestroy(level->ProjectilePoolManager);
            ObstacleManagerDestroy(level->ObstaclePoolManager);
            UIDelete(level->UI);
            fieldDelete(level->field);

            // Load game over level
            levelMgrLoad(2);
            break;
        case 2:
            UIDelete(level->UI);
            MenuDelete(level->MainMenu);

            // Load main menu level
            levelMgrLoad(0);
        }
        free(level);
    }
}

void levelMgrUnloadAll(Level* level)
{
    switch (level->currentLevelNum)
    {
    case 0:
        MenuDelete(level->MainMenu);
        break;
    case 1:
        FinalScore = level->TotalScore;
        RobotronDelete(level->Robotron_Obj);
        EnemyManagerDestroy(level->EnemyPoolManager);
        ProjectileManagerDestroy(level->ProjectilePoolManager);
        ObstacleManagerDestroy(level->ObstaclePoolManager);
        UIDelete(level->UI);
        fieldDelete(level->field);
        break;
    case 2:
        UIDelete(level->UI);
        MenuDelete(level->MainMenu);
        break;
    }
    free(level);
}

void DisableGrunt(Object* Obj)
{
    assert(Obj != NULL);
    if (Obj->objtype != EnemyGrunt) return;

    soundPlay(_gruntSoundId);
    ProjectileManagerSpawnExplosion(level->ProjectilePoolManager, Obj->position);

    level->TotalScore += level->GruntScore;
    level->UI->ScoreVal = level->TotalScore;

    level->ScoreToLifeIncrement += level->GruntScore;

    if (level->ScoreToLifeIncrement >= level->LifeIncrementScore)
    {
        level->ScoreToLifeIncrement = 0;
        IncreaseLives(level->Robotron_Obj);
        level->UI->LivesVal = GetCurrentLives(level->Robotron_Obj);
    }

    Grunt* Grunt_Obj = (Grunt*)Obj;

    EnemyManagerRecycleGrunt(level->EnemyPoolManager, Grunt_Obj);
}

void DisableHulk(Object* Obj)
{
    //soundPlay(_hulkSoundId);
}

void DisableSpheriod(Object* Obj)
{
    assert(Obj != NULL);
    if (Obj->objtype != EnemySpheroid) return;

    soundPlay(_spheroidSoundId);
    ProjectileManagerSpawnExplosion(level->ProjectilePoolManager, Obj->position);

    level->TotalScore += level->SpheroidScore;
    level->UI->ScoreVal = level->TotalScore;

    level->ScoreToLifeIncrement += level->SpheroidScore;

    if (level->ScoreToLifeIncrement >= level->LifeIncrementScore)
    {
        level->ScoreToLifeIncrement = 0;
        IncreaseLives(level->Robotron_Obj);
        level->UI->LivesVal = GetCurrentLives(level->Robotron_Obj);
    }

    Spheroid* Spheriod_Obj = (Spheroid*)Obj;

    EnemyManagerRecycleSpheroid(level->EnemyPoolManager, Spheriod_Obj);
}

void DisableObstacle(Object* Obj)
{
    assert(Obj != NULL);
    if (Obj->objtype != Obstacle) return;

    if (Obj->objSubType == SimpleObs)
    {
        soundPlay(_quarkSoundId);

        SOBS* SOBS_Obj = (SOBS*)Obj;
        ProjectileManagerRecycleSOBS(level->ProjectilePoolManager, SOBS_Obj);
        level->TotalScore += level->SOBSScore;
        level->UI->ScoreVal = level->TotalScore;

        level->ScoreToLifeIncrement += level->SOBSScore;
    }
    else if (Obj->objSubType == ComplexObs)
    {
        soundPlay(_quarkSoundId);

        COBS* COBS_Obj = (COBS*)Obj;
        ProjectileManagerRecycleCOBS(level->ProjectilePoolManager, COBS_Obj);
        level->TotalScore += level->COBSScore;
        level->UI->ScoreVal = level->TotalScore;

        level->ScoreToLifeIncrement += level->COBSScore;
    }
    else if (Obj->objSubType == StaticObs)
    {
        OBS* OBS_Obj = (OBS*)Obj;
        ObstacleManagerRecycleOBS(level->ObstaclePoolManager, OBS_Obj);
        level->TotalScore += level->StaticObsScore;
        level->UI->ScoreVal = level->TotalScore;

        level->ScoreToLifeIncrement += level->StaticObsScore;
    }
    else if (Obj->objSubType == StaticObs2084)
    {
        OBS_2084* OBS_2084_Obj = (OBS_2084*)Obj;
        ObstacleManagerRecycle_2084_OBS(level->ObstaclePoolManager, OBS_2084_Obj);
        level->TotalScore += level->StaticObs2084Score;
        level->UI->ScoreVal = level->TotalScore;

        level->ScoreToLifeIncrement += level->StaticObs2084Score;
    }

    if (level->ScoreToLifeIncrement >= level->LifeIncrementScore)
    {
        level->ScoreToLifeIncrement = 0;
        IncreaseLives(level->Robotron_Obj);
        level->UI->LivesVal = GetCurrentLives(level->Robotron_Obj);
    }
}

void DisableQuark(Object* Obj)
{
    assert(Obj != NULL);
    if (Obj->objtype != EnemyQuark) return;

    soundPlay(_quarkSoundId);
    ProjectileManagerSpawnExplosion(level->ProjectilePoolManager, Obj->position);

    level->TotalScore += level->QuarkScore;
    level->UI->ScoreVal = level->TotalScore;

    level->ScoreToLifeIncrement += level->QuarkScore;

    if (level->ScoreToLifeIncrement >= level->LifeIncrementScore)
    {
        level->ScoreToLifeIncrement = 0;
        IncreaseLives(level->Robotron_Obj);
        level->UI->LivesVal = GetCurrentLives(level->Robotron_Obj);
    }

    Quark* Quark_Obj = (Quark*)Obj;

    EnemyManagerRecycleQuark(level->EnemyPoolManager, Quark_Obj);
}

void DisableTank(Object* Obj)
{
    assert(Obj != NULL);
    if (Obj->objtype != EnemyTank) return;

    ProjectileManagerSpawnExplosion(level->ProjectilePoolManager, Obj->position);

    level->TotalScore += level->TankScore;
    level->UI->ScoreVal = level->TotalScore;

    level->ScoreToLifeIncrement += level->TankScore;

    if (level->ScoreToLifeIncrement >= level->LifeIncrementScore)
    {
        level->ScoreToLifeIncrement = 0;
        IncreaseLives(level->Robotron_Obj);
        level->UI->LivesVal = GetCurrentLives(level->Robotron_Obj);
    }

    Tank* Tank_Obj = (Tank*)Obj;

    ProjectileManagerRecycleTank(level->ProjectilePoolManager, Tank_Obj);
}

void RecycleExplosion(Object* Obj)
{
    assert(Obj != NULL);
    if (Obj->objtype != UI) return;

    Explosion* Explosion_Obj = (Explosion*)Obj;

    ProjectileManagerRecycleExplosion(level->ProjectilePoolManager, Explosion_Obj);
}

void DisableEnforcer(Object* Obj)
{
    assert(Obj != NULL);
    if (Obj->objtype != EnemyEnforcer) return;

    ProjectileManagerSpawnExplosion(level->ProjectilePoolManager, Obj->position);

    level->TotalScore += level->EnforcerScore;
    level->UI->ScoreVal = level->TotalScore;

    level->ScoreToLifeIncrement += level->EnforcerScore;

    if (level->ScoreToLifeIncrement >= level->LifeIncrementScore)
    {
        level->ScoreToLifeIncrement = 0;
        IncreaseLives(level->Robotron_Obj);
        level->UI->LivesVal = GetCurrentLives(level->Robotron_Obj);
    }

    Enforcer* Enforcer_Obj = (Enforcer*)Obj;

    ProjectileManagerRecycleEnforcer(level->ProjectilePoolManager, Enforcer_Obj);
}

void ResetScore()
{
    level->TotalScore = 0;
    level->ScoreToLifeIncrement = 0;
    level->UI->ScoreVal = 0;
}

// Reads in the high score values currently in the high score file and updates it and the UI
// based on the player's current score
void CalculateHighScoreVals(const char* FileName)
{
    uint32_t HighScores[3] = { 0 }; // Array to store the three high scores
    uint32_t NumScores = 0;

    FILE* FilePtr = NULL;
    errno_t err = fopen_s(&FilePtr, FileName, "r");
    if (err == 0 && FilePtr != NULL) // File opened successfully
    {
        int Score;
        char line[256];
        while (fgets(line, sizeof(line), FilePtr))
        {
            if (sscanf_s(line, "%d", &Score) == 1)
            {
                if (NumScores < 3) // Ensure not exceeding array bounds
                {
                    HighScores[NumScores++] = Score;
                }
            }
        }
        fclose(FilePtr);
    }
    else // File could not be opened for reading
    {
        err = fopen_s(&FilePtr, FileName, "w");
        if (err == 0 && FilePtr != NULL) // File created successfully
        {
            fprintf(FilePtr, "%d\n", FinalScore); // Write the FinalScore to the file
            fprintf(FilePtr, "%d\n", 0);
            fprintf(FilePtr, "%d\n", 0);
            fclose(FilePtr);
        }
        // Update the UI with default values
        level->UI->HighScoreVals[0] = FinalScore;
        level->UI->HighScoreVals[1] = 0;
        level->UI->HighScoreVals[2] = 0;
        return;
    }

    // Calculate new highscores
    for (int i = NumScores - 1; i >= 0; i--)
    {
        if (FinalScore > HighScores[i])
        {
            if (i != NumScores - 1)
            {
                HighScores[i + 1] = HighScores[i];
            }
            HighScores[i] = FinalScore;
        }
    }

    // Write highscores
    err = fopen_s(&FilePtr, FileName, "w");
    if (err == 0 && FilePtr != NULL) // File created successfully
    {
        for (int i = 0; i < 3; i++)
        {
            fprintf(FilePtr, "%d\n", HighScores[i]); // Write highscores to the file
        }
        fclose(FilePtr);
    }

    // Update the UI high score values
    level->UI->HighScoreVals[0] = HighScores[0];
    level->UI->HighScoreVals[1] = HighScores[1];
    level->UI->HighScoreVals[2] = HighScores[2];
}

void ResetState(Robotron* Robotron_Obj)
{
    soundPlay(_deathSoundId);
    objMgrPause();
    clearScreen();
    level->UI->LivesVal = GetCurrentLives(Robotron_Obj);
}

void ResetGame(Robotron* Robotron_Obj)
{
    soundPlay(_gameOverSoundId);
    objMgrPause();
    deathtimerActive = true;
    level->UI->LivesVal = GetCurrentLives(Robotron_Obj);
}

void AllEnemiesDead()
{
    objMgrPause();
    objMgrPauseDrawing();
    wavetimerActive = true;
    soundPlay(_levelTransitionSoundId);
}