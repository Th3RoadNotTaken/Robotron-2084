#include <stdlib.h>
#include "ProjectileManager.h"
#include "assert.h"
#include "Object.h"
#include "baseTypes.h"
#include "random.h"

typedef struct ProjectileManager_t
{
	Enforcer* Enforcers[MAX_ENFORCERS];
	Tank* Tanks[MAX_TANKS];
	SOBS* Sobs[MAX_SOBS];
	COBS* Cobs[MAX_COBS];
	Explosion* Explosions[MAX_EXPLOSIONS];
	uint8_t ActiveProjectiles[PROJECTILE_TYPES];
} ProjectileManager;

// Initializes the projectile pool by creating MAX objects of each projectile type
ProjectileManager* ProjectileManagerInit(Bounds2D FieldBounds, Robotron* Robotron_Obj)
{
	ProjectileManager* Manager = malloc(sizeof(ProjectileManager));
	assert(Manager != NULL);

	EnforcerInitTextures();
	TankInitTextures();
	SOBSInitTextures();
	COBSInitTextures();
	ExplosionInitTextures();

	// Create enforcer objects
	for (int i = 0; i < MAX_ENFORCERS; i++)
	{
		Manager->Enforcers[i] = EnforcerNew((Coord2D) { 0.0f, 0.0f }, FieldBounds, Robotron_Obj, false);
	}
	Manager->ActiveProjectiles[0] = 0;

	// Create simple obstacle objects
	for (int i = 0; i < MAX_SOBS; i++)
	{
		Manager->Sobs[i] = SOBSNew((Coord2D) { 0.0f, 0.0f }, FieldBounds, false);
	}
	Manager->ActiveProjectiles[1] = 0;

	// Create complex obstacle objects
	for (int i = 0; i < MAX_COBS; i++)
	{
		Manager->Cobs[i] = COBSNew((Coord2D) { 0.0f, 0.0f }, FieldBounds, false);
	}
	Manager->ActiveProjectiles[2] = 0;

	// Create tank objects
	for (int i = 0; i < MAX_TANKS; i++)
	{
		Manager->Tanks[i] = TankNew((Coord2D) { 0.0f, 0.0f }, FieldBounds, Robotron_Obj, false);
	}
	Manager->ActiveProjectiles[3] = 0;

	// Create explosion objects
	for (int i = 0; i < MAX_EXPLOSIONS; i++)
	{
		Manager->Explosions[i] = ExplosionNew(FieldBounds, false, (Coord2D) { 0.0f, 0.0f });
	}
	Manager->ActiveProjectiles[4] = 0;

	return Manager;
}

void ProjectileManagerSpawnEnforcer(ProjectileManager* Manager, Coord2D Position)
{
	assert(Manager != NULL);

	// Prevent spawning if max enforcers are active
	if (Manager->ActiveProjectiles[0] >= MAX_ENFORCERS)
	{
		return;
	}

	for (int i = 0; i < MAX_ENFORCERS; i++)
	{
		Enforcer* Enforcer_Obj = Manager->Enforcers[i];
		assert(Enforcer_Obj != NULL);

		if (Enforcer_Obj->Obj.IsActive == false)
		{
			Enforcer_Obj->Obj.position = Position;
			Enforcer_Obj->Obj.IsActive = true;
			Enforcer_Obj->TargetPoint = Enforcer_Obj->Robotron_Obj->Obj.position;
			Enforcer_Obj->VelocityInitialized = false;
			++Manager->ActiveProjectiles[0];

			return;
		}
	}
}

void ProjectileManagerRecycleEnforcer(ProjectileManager* Manager, Enforcer* Enforcer_Obj)
{
	assert(Manager != NULL && Enforcer_Obj != NULL);

	// Deactivate the enforcer object
	Enforcer_Obj->Obj.IsActive = false;
	--Manager->ActiveProjectiles[0];
}

void ProjectileManagerRecycleAllEnforcers(ProjectileManager* Manager)
{
	assert(Manager != NULL);

	for (int i = 0; i < MAX_ENFORCERS; i++)
	{
		Enforcer* Enforcer_Obj = Manager->Enforcers[i];
		assert(Enforcer_Obj != NULL);

		if (Enforcer_Obj->Obj.IsActive == true)
		{
			Enforcer_Obj->Obj.IsActive = false;
			Enforcer_Obj->VelocityInitialized = false;
			--Manager->ActiveProjectiles[0];
		}
	}
}

void ProjectileManagerSpawnTank(ProjectileManager* Manager, Coord2D Position)
{
	assert(Manager != NULL);

	// Prevent spawning if max tanks are active
	if (Manager->ActiveProjectiles[3] >= MAX_TANKS)
	{
		return;
	}

	for (int i = 0; i < MAX_TANKS; i++)
	{
		Tank* Tank_Obj = Manager->Tanks[i];
		assert(Tank_Obj != NULL);

		if (Tank_Obj->Obj.IsActive == false)
		{
			Tank_Obj->Obj.position = Position;
			Tank_Obj->Obj.IsActive = true;
			Tank_Obj->TargetPoint = Tank_Obj->Robotron_Obj->Obj.position;
			Tank_Obj->VelocityInitialized = false;
			++Manager->ActiveProjectiles[3];

			return;
		}
	}
}

void ProjectileManagerRecycleTank(ProjectileManager* Manager, Tank* Tank_Obj)
{
	assert(Manager != NULL && Tank_Obj != NULL);

	// Deactivate the tank object
	Tank_Obj->Obj.IsActive = false;
	--Manager->ActiveProjectiles[3];
}

void ProjectileManagerRecycleAllTanks(ProjectileManager* Manager)
{
	assert(Manager != NULL);

	for (int i = 0; i < MAX_TANKS; i++)
	{
		Tank* Tank_Obj = Manager->Tanks[i];
		assert(Tank_Obj != NULL);

		if (Tank_Obj->Obj.IsActive == true)
		{
			Tank_Obj->Obj.IsActive = false;
			Tank_Obj->VelocityInitialized = false;
			--Manager->ActiveProjectiles[3];
		}
	}
}

void ProjectileManagerSpawnSOBS(ProjectileManager* Manager, Coord2D Position)
{
	assert(Manager != NULL);

	// Prevent spawning if max simple obstacles are active
	if (Manager->ActiveProjectiles[1] >= MAX_SOBS)
	{
		return;
	}

	for (int i = 0; i < MAX_SOBS; i++)
	{
		SOBS* SOBS_Obj = Manager->Sobs[i];
		assert(SOBS_Obj != NULL);

		if (SOBS_Obj->Obj.IsActive == false)
		{
			SOBS_Obj->Obj.position = Position;
			SOBS_Obj->Obj.IsActive = true;
			SOBS_Obj->VelocityInitialized = false;
			SOBS_Obj->Obj.CanSpawn = false;
			SOBS_Obj->ElapsedSpawnTime = 0.0f;
			SOBS_Obj->SpawnDelay = randGetFloat(SOBS_SPAWN_TIME_MIN_DURATION, SOBS_SPAWN_TIME_MAX_DURATION);
			++Manager->ActiveProjectiles[1];

			return;
		}
	}
}

void ProjectileManagerRecycleSOBS(ProjectileManager* Manager, SOBS* SOBS_Obj)
{
	assert(Manager != NULL && SOBS_Obj != NULL);

	// Deactivate the simple obstacle object
	SOBS_Obj->Obj.IsActive = false;
	--Manager->ActiveProjectiles[1];
}

void ProjectileManagerRecycleAllSOBS(ProjectileManager* Manager)
{
	assert(Manager != NULL);

	for (int i = 0; i < MAX_SOBS; i++)
	{
		SOBS* SOBS_Obj = Manager->Sobs[i];
		assert(SOBS_Obj != NULL);

		if (SOBS_Obj->Obj.IsActive == true)
		{
			SOBS_Obj->Obj.IsActive = false;
			SOBS_Obj->VelocityInitialized = false;
			SOBS_Obj->Obj.CanSpawn = false;
			--Manager->ActiveProjectiles[1];
		}
	}
}

void ProjectileManagerRespawnSOBS(ProjectileManager* Manager, Coord2D* RandSpawnPoints)
{
	assert(Manager != NULL && RandSpawnPoints != NULL);

	int spawnPointIdx = 0;
	for (int i = 0; i < MAX_SOBS; i++)
	{
		SOBS* SOBS_Obj = Manager->Sobs[i];
		assert(SOBS_Obj != NULL);

		if (SOBS_Obj->Obj.IsActive == true)
		{
			SOBS_Obj->Obj.position = RandSpawnPoints[spawnPointIdx++];
			SOBS_Obj->VelocityInitialized = false;
			SOBS_Obj->Obj.CanSpawn = false;
			SOBS_Obj->ElapsedSpawnTime = 0.0f;
			SOBS_Obj->SpawnDelay = randGetFloat(SOBS_SPAWN_TIME_MIN_DURATION, SOBS_SPAWN_TIME_MAX_DURATION);
		}
	}
}

void ProjectileManagerSpawnCOBS(ProjectileManager* Manager, Coord2D Position)
{
	assert(Manager != NULL);

	// Prevent spawning if max simple obstacles are active
	if (Manager->ActiveProjectiles[2] >= MAX_COBS)
	{
		return;
	}

	for (int i = 0; i < MAX_COBS; i++)
	{
		COBS* COBS_Obj = Manager->Cobs[i];
		assert(COBS_Obj != NULL);

		if (COBS_Obj->Obj.IsActive == false)
		{
			COBS_Obj->Obj.position = Position;
			COBS_Obj->Obj.IsActive = true;
			COBS_Obj->Obj.CanSpawn = false;
			COBS_Obj->ElapsedSpawnTime = 0.0f;
			COBS_Obj->SpawnDelay = randGetFloat(COBS_SPAWN_TIME_MIN_DURATION, COBS_SPAWN_TIME_MAX_DURATION);
			COBS_Obj->NextWaypoint = GenerateRandomWaypoint(COBS_Obj->Bounds);
			COBS_Obj->FlyElapsedTime = 0.0f;
			COBS_Obj->FlyDuration = randGetFloat(COBS_MIN_FLY_DURATION, COBS_MAX_FLY_DURATION);
			float randVel = randGetFloat(COBS_MIN_VELOCITY, COBS_MAX_VELOCITY);
			COBS_Obj->Obj.velocity = (Coord2D){ randVel , randVel };
			++Manager->ActiveProjectiles[2];

			return;
		}
	}
}

void ProjectileManagerRecycleCOBS(ProjectileManager* Manager, COBS* COBS_Obj)
{
	assert(Manager != NULL && COBS_Obj != NULL);

	// Deactivate the complex obstacle object
	COBS_Obj->Obj.IsActive = false;
	--Manager->ActiveProjectiles[2];
}

void ProjectileManagerRecycleAllCOBS(ProjectileManager* Manager)
{
	assert(Manager != NULL);

	for (int i = 0; i < MAX_COBS; i++)
	{
		COBS* COBS_Obj = Manager->Cobs[i];
		assert(COBS_Obj != NULL);

		if (COBS_Obj->Obj.IsActive == true)
		{
			COBS_Obj->Obj.IsActive = false;
			COBS_Obj->Obj.CanSpawn = false;
			--Manager->ActiveProjectiles[2];
		}
	}
}

void ProjectileManagerRespawnCOBS(ProjectileManager* Manager, Coord2D* RandSpawnPoints)
{
	assert(Manager != NULL && RandSpawnPoints != NULL);

	int spawnPointIdx = 0;
	for (int i = 0; i < MAX_COBS; i++)
	{
		COBS* COBS_Obj = Manager->Cobs[i];
		assert(COBS_Obj != NULL);

		if (COBS_Obj->Obj.IsActive == true)
		{
			COBS_Obj->Obj.position = RandSpawnPoints[spawnPointIdx++];
			COBS_Obj->Obj.CanSpawn = false;
			COBS_Obj->ElapsedSpawnTime = 0.0f;
			COBS_Obj->SpawnDelay = randGetFloat(COBS_SPAWN_TIME_MIN_DURATION, COBS_SPAWN_TIME_MAX_DURATION);
			COBS_Obj->NextWaypoint = GenerateRandomWaypoint(COBS_Obj->Bounds);
			COBS_Obj->FlyDuration = randGetFloat(COBS_MIN_FLY_DURATION, COBS_MAX_FLY_DURATION);
			COBS_Obj->FlyElapsedTime = 0.0f;
		}
	}
}

void ProjectileManagerSpawnExplosion(ProjectileManager* Manager, Coord2D Position)
{
	assert(Manager != NULL);

	// Prevent spawning if max explosions are active
	if (Manager->ActiveProjectiles[4] >= MAX_EXPLOSIONS)
	{
		return;
	}

	for (int i = 0; i < MAX_EXPLOSIONS; i++)
	{
		Explosion* Explosion_Obj = Manager->Explosions[i];
		assert(Explosion_Obj != NULL);

		if (Explosion_Obj->Obj.IsActive == false)
		{
			Explosion_Obj->Obj.position = Position;
			Explosion_Obj->Obj.IsActive = true;
			Explosion_Obj->shouldPlayAnim = true;
			int randAnimIndex = randGetInt(0, EXP_ANIM_TYPES);
			Explosion_Obj->ExplosionAnimIndex = randAnimIndex;
			++Manager->ActiveProjectiles[4];

			return;
		}
	}
}

void ProjectileManagerRecycleExplosion(ProjectileManager* Manager, Explosion* Explosion_Obj)
{
	assert(Manager != NULL && Explosion_Obj != NULL);

	// Deactivate the explosion obstacle object
	Explosion_Obj->Obj.IsActive = false;
	Explosion_Obj->shouldPlayAnim = false;
	--Manager->ActiveProjectiles[4];
}

void ProjectileManagerRecycleAllExplosions(ProjectileManager* Manager)
{
	assert(Manager != NULL);

	for (int i = 0; i < MAX_EXPLOSIONS; i++)
	{
		Explosion* Explosion_Obj = Manager->Explosions[i];
		assert(Explosion_Obj != NULL);

		if (Explosion_Obj->Obj.IsActive == true)
		{
			Explosion_Obj->Obj.IsActive = false;
			Explosion_Obj->shouldPlayAnim = false;
			--Manager->ActiveProjectiles[4];
		}
	}
}

void RecycleAllProjectiles(ProjectileManager* Manager)
{
	assert(Manager != NULL);

	ProjectileManagerRecycleAllEnforcers(Manager);
	ProjectileManagerRecycleAllSOBS(Manager);
	ProjectileManagerRecycleAllCOBS(Manager);
	ProjectileManagerRecycleAllTanks(Manager);
	ProjectileManagerRecycleAllExplosions(Manager);
}

void ProjectileManagerDestroy(ProjectileManager* Manager)
{
	assert(Manager != NULL);

	// Delete every enforcer
	for (int i = 0; i < MAX_ENFORCERS; ++i) {
		EnforcerDelete(Manager->Enforcers[i]);
		Manager->Enforcers[i] = NULL;
	}
	Manager->ActiveProjectiles[0] = 0;

	// Delete every simple obstacle
	for (int i = 0; i < MAX_SOBS; ++i) {
		SOBSDelete(Manager->Sobs[i]);
		Manager->Sobs[i] = NULL;
	}
	Manager->ActiveProjectiles[1] = 0;

	// Delete every complex obstacle
	for (int i = 0; i < MAX_COBS; ++i) {
		COBSDelete(Manager->Cobs[i]);
		Manager->Cobs[i] = NULL;
	}
	Manager->ActiveProjectiles[2] = 0;

	// Delete every tank
	for (int i = 0; i < MAX_TANKS; ++i) {
		TankDelete(Manager->Tanks[i]);
		Manager->Tanks[i] = NULL;
	}
	Manager->ActiveProjectiles[3] = 0;

	// Delete every explosion
	for (int i = 0; i < MAX_EXPLOSIONS; ++i) {
		ExplosionDelete(Manager->Explosions[i]);
		Manager->Explosions[i] = NULL;
	}
	Manager->ActiveProjectiles[4] = 0;

	free(Manager);
	Manager = NULL;
}