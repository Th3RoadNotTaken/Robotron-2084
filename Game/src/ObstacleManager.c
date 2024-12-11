#include <stdlib.h>
#include "assert.h"
#include "Object.h"
#include "baseTypes.h"
#include "random.h"
#include "ObstacleManager.h"

typedef struct ObstacleManager_t
{
	OBS* OBS_Objects[MAX_STATIC_OBS];
	OBS_2084* OBS_2084_Objects[MAX_STATIC_2048_OBS];
	uint8_t ActiveObstacles[OBS_TYPES];
} ObstacleManager;

// Initializes the obstacle pool with MAX objects of each obstacle type
ObstacleManager* ObstacleManagerInit(Bounds2D FieldBounds)
{
	ObstacleManager* Manager = malloc(sizeof(ObstacleManager));
	assert(Manager != NULL);

	OBSInitTextures();

	// Create static obstacle objects
	for (int i = 0; i < MAX_STATIC_OBS; i++)
	{
		Manager->OBS_Objects[i] = OBSNew((Coord2D) { 0.0f, 0.0f }, FieldBounds, false);
	}
	Manager->ActiveObstacles[0] = 0;

	// Create static 2084 obstacle objects
	for (int i = 0; i < MAX_STATIC_2048_OBS; i++)
	{
		Manager->OBS_2084_Objects[i] = OBS_2084_New((Coord2D) { 0.0f, 0.0f }, FieldBounds, false);
	}
	Manager->ActiveObstacles[1] = 0;

	return Manager;
}

void ObstacleManagerSpawnOBS(ObstacleManager* Manager, Coord2D Position)
{
	assert(Manager != NULL);

	// Prevent spawning if max obstacles are active
	if (Manager->ActiveObstacles[0] >= MAX_STATIC_OBS)
	{
		return;
	}

	for (int i = 0; i < MAX_STATIC_OBS; i++)
	{
		OBS* OBS_Obj = Manager->OBS_Objects[i];
		assert(OBS_Obj != NULL);

		if (OBS_Obj->Obj.IsActive == false)
		{
			OBS_Obj->Obj.position = Position;
			OBS_Obj->Obj.IsActive = true;
			OBS_Obj->CurrentFrame = randGetInt(0, OBS_NUM);
			++Manager->ActiveObstacles[0];

			return;
		}
	}
}

void ObstacleManagerRecycleOBS(ObstacleManager* Manager, OBS* OBS_Obj)
{
	assert(Manager != NULL);

	// Deactivate the obstacle object
	OBS_Obj->Obj.IsActive = false;
	--Manager->ActiveObstacles[0];
}

void ObstacleManagerRecycleAllOBS(ObstacleManager* Manager)
{
	assert(Manager != NULL);

	for (int i = 0; i < MAX_STATIC_OBS; i++)
	{
		OBS* OBS_Obj = Manager->OBS_Objects[i];
		assert(OBS_Obj != NULL);

		if (OBS_Obj->Obj.IsActive == true)
		{
			OBS_Obj->Obj.IsActive = false;
			--Manager->ActiveObstacles[0];
		}
	}
}

void ObstacleManagerRespawnOBS(ObstacleManager* Manager, Coord2D* RandSpawnPoints)
{
	assert(Manager != NULL && RandSpawnPoints != NULL);

	int spawnPointIdx = 0;
	for (int i = 0; i < MAX_STATIC_OBS; i++)
	{
		OBS* OBS_Obj = Manager->OBS_Objects[i];
		assert(OBS_Obj != NULL);

		if (OBS_Obj->Obj.IsActive == true)
		{
			OBS_Obj->Obj.position = RandSpawnPoints[spawnPointIdx++];
		}
	}
}

void ObstacleManagerSpawn_2084_OBS(ObstacleManager* Manager, Coord2D Position)
{
	assert(Manager != NULL);

	// Prevent spawning if max obstacles are active
	if (Manager->ActiveObstacles[1] >= MAX_STATIC_2048_OBS)
	{
		return;
	}

	for (int i = 0; i < MAX_STATIC_2048_OBS; i++)
	{
		OBS_2084* OBS_2084_Obj = Manager->OBS_2084_Objects[i];
		assert(OBS_2084_Obj != NULL);

		if (OBS_2084_Obj->Obj.IsActive == false)
		{
			OBS_2084_Obj->Obj.position = Position;
			OBS_2084_Obj->Obj.IsActive = true;
			++Manager->ActiveObstacles[1];

			return;
		}
	}
}

void ObstacleManagerRecycle_2084_OBS(ObstacleManager* Manager, OBS_2084* OBS_2084_Obj)
{
	assert(Manager != NULL);

	// Deactivate the obstacle object
	OBS_2084_Obj->Obj.IsActive = false;
	--Manager->ActiveObstacles[1];
}

void ObstacleManagerRecycleAll_2084_OBS(ObstacleManager* Manager)
{
	assert(Manager != NULL);

	for (int i = 0; i < MAX_STATIC_2048_OBS; i++)
	{
		OBS_2084* OBS_2084_Obj = Manager->OBS_2084_Objects[i];
		assert(OBS_2084_Obj != NULL);

		if (OBS_2084_Obj->Obj.IsActive == true)
		{
			OBS_2084_Obj->Obj.IsActive = false;
			--Manager->ActiveObstacles[1];
		}
	}
}

void ObstacleManagerRespawn_2084_OBS(ObstacleManager* Manager, Coord2D* RandSpawnPoints)
{
	assert(Manager != NULL && RandSpawnPoints != NULL);

	int spawnPointIdx = 0;
	for (int i = 0; i < MAX_STATIC_2048_OBS; i++)
	{
		OBS_2084* OBS_2084_Obj = Manager->OBS_2084_Objects[i];
		assert(OBS_2084_Obj != NULL);

		if (OBS_2084_Obj->Obj.IsActive == true)
		{
			OBS_2084_Obj->Obj.position = RandSpawnPoints[spawnPointIdx++];
		}
	}
}

void ObstacleManagerDestroy(ObstacleManager* Manager)
{
	assert(Manager != NULL);

	// Delete every static obstacle
	for (int i = 0; i < MAX_STATIC_OBS; ++i) {
		OBSDelete(Manager->OBS_Objects[i]);
		Manager->OBS_Objects[i] = NULL;
	}
	Manager->ActiveObstacles[0] = 0;

	// Delete every static 2084 obstacle
	for (int i = 0; i < MAX_STATIC_2048_OBS; ++i) {
		OBS_2084_Delete(Manager->OBS_2084_Objects[i]);
		Manager->OBS_2084_Objects[i] = NULL;
	}
	Manager->ActiveObstacles[1] = 0;

	free(Manager);
	Manager = NULL;
}
