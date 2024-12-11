#pragma once
#include "Grunt.h"
#include "Hulk.h"
#include "Spheroid.h"
#include "Quark.h"
#include "Robotron.h"
#include "ProjectileManager.h"
#include "stdbool.h"

#define MAX_GRUNTS	   50
#define MAX_HULKS      30
#define MAX_SPHEROIDS  30
#define MAX_QUARKS	   30

#define ENEMY_TYPES    4

typedef struct EnemyManager_t EnemyManager;

typedef void (*AllEnemiesKilled)();
void EnemyManagerSetAllEnemiesKilledCB(AllEnemiesKilled cb);
void EnemyManagerClearAllEnemiesKilledCB();

EnemyManager* EnemyManagerInit(Bounds2D FieldBounds, Robotron* Robotron_Obj, ProjectileManager* ProjManager);

void EnemyManagerSpawnGrunt(EnemyManager* Manager, Coord2D Position);
void EnemyManagerRespawnGrunts(EnemyManager* Manager, Coord2D* RandSpawnPoints);
void EnemyManagerRecycleGrunt(EnemyManager* Manager, Grunt* Grunt_Obj);
void EnemyManagerRecycleAllGrunts(EnemyManager* Manager);

void EnemyManagerSpawnHulk(EnemyManager* Manager, Coord2D Position);
void EnemyManagerRespawnHulks(EnemyManager* Manager, Coord2D* RandSpawnPoints);
void EnemyManagerRecycleAllHulks(EnemyManager* Manager);

void EnemyManagerSpawnSpheroid(EnemyManager* Manager, Coord2D Position);
void EnemyManagerRespawnSpheroids(EnemyManager* Manager, Coord2D* RandSpawnPoints);
void EnemyManagerRecycleSpheroid(EnemyManager* Manager, Spheroid* Spheroid_Obj);
void EnemyManagerRecycleAllSpheroids(EnemyManager* Manager);

void EnemyManagerSpawnQuark(EnemyManager* Manager, Coord2D Position);
void EnemyManagerRespawnQuarks(EnemyManager* Manager, Coord2D* RandSpawnPoints);
void EnemyManagerRecycleQuark(EnemyManager* Manager, Quark* Quark_Obj);
void EnemyManagerRecycleAllQuarks(EnemyManager* Manager);

void RecycleAllEnemies(EnemyManager* Manager);
void EnemyManagerDestroy(EnemyManager* Manager);