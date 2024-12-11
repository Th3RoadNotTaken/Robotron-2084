#pragma once
#include "Enforcer.h"
#include "Tank.h"
#include "SimpleObstacle.h"
#include "ComplexObstacle.h"
#include "Explosion.h"
#include "Robotron.h"
#include "stdbool.h"

#define MAX_ENFORCERS		20
#define MAX_TANKS			30
#define MAX_SOBS		    50
#define MAX_COBS		    50
#define MAX_EXPLOSIONS		50
#define PROJECTILE_TYPES	5

typedef struct ProjectileManager_t ProjectileManager;

ProjectileManager* ProjectileManagerInit(Bounds2D FieldBounds, Robotron* Robotron_Obj);

void ProjectileManagerSpawnEnforcer(ProjectileManager* Manager, Coord2D Position);
void ProjectileManagerRecycleEnforcer(ProjectileManager* Manager, Enforcer* Enforcer_Obj);
void ProjectileManagerRecycleAllEnforcers(ProjectileManager* Manager);

void ProjectileManagerSpawnTank(ProjectileManager* Manager, Coord2D Position);
void ProjectileManagerRecycleTank(ProjectileManager* Manager, Tank* Tank_Obj);
void ProjectileManagerRecycleAllTanks(ProjectileManager* Manager);

void ProjectileManagerSpawnSOBS(ProjectileManager* Manager, Coord2D Position);
void ProjectileManagerRecycleSOBS(ProjectileManager* Manager, SOBS* SOBS_Obj);
void ProjectileManagerRecycleAllSOBS(ProjectileManager* Manager);
void ProjectileManagerRespawnSOBS(ProjectileManager* Manager, Coord2D* RandSpawnPoints);

void ProjectileManagerSpawnCOBS(ProjectileManager* Manager, Coord2D Position);
void ProjectileManagerRecycleCOBS(ProjectileManager* Manager, COBS* COBS_Obj);
void ProjectileManagerRecycleAllCOBS(ProjectileManager* Manager);
void ProjectileManagerRespawnCOBS(ProjectileManager* Manager, Coord2D* RandSpawnPoints);

void ProjectileManagerSpawnExplosion(ProjectileManager* Manager, Coord2D Position);
void ProjectileManagerRecycleExplosion(ProjectileManager* Manager, Explosion* Explosion_Obj);
void ProjectileManagerRecycleAllExplosions(ProjectileManager* Manager);

void RecycleAllProjectiles(ProjectileManager* Manager);

void ProjectileManagerDestroy(ProjectileManager* Manager);