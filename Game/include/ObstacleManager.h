#pragma once
#include "StaticObstacles.h"
#include "stdbool.h"

#define MAX_STATIC_OBS		    70
#define MAX_STATIC_2048_OBS		70
#define OBS_TYPES				2	

typedef struct ObstacleManager_t ObstacleManager;

ObstacleManager* ObstacleManagerInit(Bounds2D FieldBounds);

void ObstacleManagerSpawnOBS(ObstacleManager* Manager, Coord2D Position);
void ObstacleManagerRecycleOBS(ObstacleManager* Manager, OBS* OBS_Obj);
void ObstacleManagerRecycleAllOBS(ObstacleManager* Manager);
void ObstacleManagerRespawnOBS(ObstacleManager* Manager, Coord2D* RandSpawnPoints);

void ObstacleManagerSpawn_2084_OBS(ObstacleManager* Manager, Coord2D Position);
void ObstacleManagerRecycle_2084_OBS(ObstacleManager* Manager, OBS_2084* OBS_2084_Obj);
void ObstacleManagerRecycleAll_2084_OBS(ObstacleManager* Manager);
void ObstacleManagerRespawn_2084_OBS(ObstacleManager* Manager, Coord2D* RandSpawnPoints);

void ObstacleManagerDestroy(ObstacleManager* Manager);