#include <stdlib.h>
#include "assert.h"
#include "Object.h"
#include "baseTypes.h"
#include "EnemyManager.h"
#include "random.h"

typedef struct EnemyManager_t
{
	Grunt* Grunts[MAX_GRUNTS];
	Hulk* Hulks[MAX_HULKS];
	Spheroid* Spheroids[MAX_SPHEROIDS];
	Quark* Quarks[MAX_QUARKS];
	uint8_t ActiveEnemies[ENEMY_TYPES];
	uint16_t TotalEnemies;
} EnemyManager;

static AllEnemiesKilled _allEnemiesKilledCB = NULL;
static void _enemyManagerTriggerAllEnemiesDeadCB();

void EnemyManagerSetAllEnemiesKilledCB(AllEnemiesKilled cb)
{
	_allEnemiesKilledCB = cb;
}

void EnemyManagerClearAllEnemiesKilledCB()
{
	_allEnemiesKilledCB = NULL;
}

// Initializes the enemy pool by creating MAX objects of each enemy type
EnemyManager* EnemyManagerInit(Bounds2D FieldBounds, Robotron* Robotron_Obj, ProjectileManager* ProjManager)
{
	EnemyManager* Manager = malloc(sizeof(EnemyManager));
	assert(Manager != NULL);

	GruntInitTextures();
	HulkInitTextures();
	SpheroidInitTextures();
	QuarkInitTextures();

	// Create grunt objects
	for (int i = 0; i < MAX_GRUNTS; i++)
	{
		Manager->Grunts[i] = GruntNew((Coord2D) { 0.0f, 0.0f }, FieldBounds, Robotron_Obj, false);
	}
	Manager->ActiveEnemies[0] = 0;

	// Create hulk objects
	for (int i = 0; i < MAX_HULKS; i++)
	{
		Manager->Hulks[i] = HulkNew((Coord2D) { 0.0f, 0.0f }, FieldBounds, Robotron_Obj, false);
	}
	Manager->ActiveEnemies[1] = 0;

	// Create spheroid objects
	for (int i = 0; i < MAX_SPHEROIDS; i++)
	{
		Manager->Spheroids[i] = SpheroidNew((Coord2D) { 0.0f, 0.0f }, FieldBounds, Robotron_Obj, false, ProjManager);
	}
	Manager->ActiveEnemies[2] = 0;

	// Create quark objects
	for (int i = 0; i < MAX_QUARKS; i++)
	{
		Manager->Quarks[i] = QuarkNew((Coord2D) { 0.0f, 0.0f }, FieldBounds, Robotron_Obj, false, ProjManager);
	}
	Manager->ActiveEnemies[3] = 0;

	Manager->TotalEnemies = 0;

	return Manager;
}

void EnemyManagerSpawnGrunt(EnemyManager* Manager, Coord2D Position)
{
	assert(Manager != NULL);

	// Prevent spawning if max grunts are active
	if (Manager->ActiveEnemies[0] >= MAX_GRUNTS)
	{
		return;
	}

	for (int i = 0; i < MAX_GRUNTS; i++)
	{
		Grunt* Grunt_Obj = Manager->Grunts[i];
		assert(Grunt_Obj != NULL);

		if (Grunt_Obj->Obj.IsActive == false)
		{
			Grunt_Obj->Obj.position = Position;
			Grunt_Obj->Obj.IsActive = true;
			Grunt_Obj->MoveElapsedTime = 0;
			float randDuration = randGetFloat(GRUNT_MOVE_MIN_DURATION, GRUNT_MOVE_MAX_DURATION);
			Grunt_Obj->MoveDuration = randDuration;
			Grunt_Obj->StopElapsedTime = 0;
			randDuration = randGetFloat(GRUNT_STOP_MIN_DURATION, GRUNT_STOP_MAX_DURATION);
			Grunt_Obj->StopDuration = randDuration;
			++Manager->ActiveEnemies[0];
			++Manager->TotalEnemies;

			return;
		}
	}
}

void EnemyManagerRespawnGrunts(EnemyManager* Manager, Coord2D* RandSpawnPoints)
{
	assert(Manager != NULL && RandSpawnPoints != NULL);

	int spawnPointIdx = 0;
	for (int i = 0; i < MAX_GRUNTS; i++)
	{
		Grunt* Grunt_Obj = Manager->Grunts[i];
		assert(Grunt_Obj != NULL);

		if (Grunt_Obj->Obj.IsActive == true)
		{
			Grunt_Obj->Obj.position = RandSpawnPoints[spawnPointIdx++];
			Grunt_Obj->MoveElapsedTime = 0;
			float randDuration = randGetFloat(GRUNT_MOVE_MIN_DURATION, GRUNT_MOVE_MAX_DURATION);
			Grunt_Obj->MoveDuration = randDuration;
			Grunt_Obj->StopElapsedTime = 0;
			randDuration = randGetFloat(GRUNT_STOP_MIN_DURATION, GRUNT_STOP_MAX_DURATION);
			Grunt_Obj->StopDuration = randDuration;
		}
	}
}

void EnemyManagerRecycleGrunt(EnemyManager* Manager, Grunt* Grunt_Obj)
{
	assert(Manager != NULL && Grunt_Obj != NULL);

	// Deactivate the Grunt object
	Grunt_Obj->Obj.IsActive = false;
	--Manager->ActiveEnemies[0];
	--Manager->TotalEnemies;

	if (Manager->TotalEnemies == 0)
		_enemyManagerTriggerAllEnemiesDeadCB();
}

void EnemyManagerRecycleAllGrunts(EnemyManager* Manager)
{
	assert(Manager != NULL);

	// Deactivate the grunt objects
	for (int i = 0; i < MAX_GRUNTS; i++)
	{
		Grunt* Grunt_Obj = Manager->Grunts[i];
		assert(Grunt_Obj != NULL);

		if (Grunt_Obj->Obj.IsActive == true)
		{
			Grunt_Obj->Obj.IsActive = false;
			--Manager->ActiveEnemies[0];
		}
	}
}

void EnemyManagerSpawnHulk(EnemyManager* Manager, Coord2D Position)
{
	assert(Manager != NULL);

	// Prevent spawning if max hulks are active
	if (Manager->ActiveEnemies[1] >= MAX_HULKS)
	{
		return;
	}

	for (int i = 0; i < MAX_HULKS; i++)
	{
		Hulk* Hulk_Obj = Manager->Hulks[i];
		assert(Hulk_Obj != NULL);

		if (Hulk_Obj->Obj.IsActive == false)
		{
			// Get random direction
			enum Direction HulkDir = (enum Direction)(rand() % 4);

			// Get random walk time
			float randWalkDuration = randGetFloat(HULK_MIN_WALK_DURATION, HULK_MAX_WALK_DURATION);

			Hulk_Obj->Obj.position = Position;
			Hulk_Obj->Obj.direction = HulkDir;
			Hulk_Obj->WalkDuration = randWalkDuration;
			float randVel = randGetFloat(HULK_MIN_VELOCITY, HULK_MAX_VELOCITY);
			Hulk_Obj->Obj.velocity = (Coord2D){ randVel ,randVel };
			Hulk_Obj->Obj.IsActive = true;
			++Manager->ActiveEnemies[1];

			return;
		}
	}
}

void EnemyManagerRespawnHulks(EnemyManager* Manager, Coord2D* RandSpawnPoints)
{
	assert(Manager != NULL && RandSpawnPoints != NULL);

	int spawnPointIdx = 0;
	for (int i = 0; i < MAX_HULKS; i++)
	{
		Hulk* Hulk_Obj = Manager->Hulks[i];
		assert(Hulk_Obj != NULL);

		if (Hulk_Obj->Obj.IsActive == true)
		{
			Hulk_Obj->Obj.position = RandSpawnPoints[spawnPointIdx++];
		}
	}
}

void EnemyManagerRecycleAllHulks(EnemyManager* Manager)
{
	assert(Manager != NULL);

	// Deactivate the Hulk objects
	for (int i = 0; i < MAX_HULKS; i++)
	{
		Hulk* Hulk_Obj = Manager->Hulks[i];
		assert(Hulk_Obj != NULL);

		if (Hulk_Obj->Obj.IsActive == true)
		{
			Hulk_Obj->Obj.IsActive = false;
			--Manager->ActiveEnemies[1];
		}
	}
}

void EnemyManagerSpawnSpheroid(EnemyManager* Manager, Coord2D Position)
{
	assert(Manager != NULL);

	// Prevent spawning if max spheroids are active
	if (Manager->ActiveEnemies[2] >= MAX_SPHEROIDS)
	{
		return;
	}

	for (int i = 0; i < MAX_SPHEROIDS; i++)
	{
		Spheroid* Spheroid_Obj = Manager->Spheroids[i];
		assert(Spheroid_Obj != NULL);

		if (Spheroid_Obj->Obj.IsActive == false)
		{
			Spheroid_Obj->Obj.position = Position;
			Spheroid_Obj->Obj.IsActive = true;
			Spheroid_Obj->NextWaypoint = GenerateRandomWaypoint(Spheroid_Obj->Bounds);
			Spheroid_Obj->FlyElapsedTime = 0.0f;
			Spheroid_Obj->FlyDuration = randGetFloat(SPHEROID_MIN_FLY_DURATION, SPHEROID_MAX_FLY_DURATION);
			Spheroid_Obj->EnforcerSpawnElapsedTime = 0.0f;
			Spheroid_Obj->EnforcerSpawnDelay = randGetFloat(SPHEROID_MIN_ENFORCER_SPAWN_DELAY, SPHEROID_MAX_ENFORCER_SPAWN_DELAY);
			float randVel = randGetFloat(SPHEROID_MIN_VELOCITY, SPHEROID_MAX_VELOCITY);
			Spheroid_Obj->Obj.velocity = (Coord2D){ randVel , randVel };
			Spheroid_Obj->PlaySpawnAnim = true;
			++Manager->ActiveEnemies[2];
			++Manager->TotalEnemies;

			return;
		}
	}
}

void EnemyManagerRespawnSpheroids(EnemyManager* Manager, Coord2D* RandSpawnPoints)
{
	assert(Manager != NULL && RandSpawnPoints != NULL);

	int spawnPointIdx = 0;
	for (int i = 0; i < MAX_SPHEROIDS; i++)
	{
		Spheroid* Spheroid_Obj = Manager->Spheroids[i];
		assert(Spheroid_Obj != NULL);

		if (Spheroid_Obj->Obj.IsActive == true)
		{
			Spheroid_Obj->Obj.position = RandSpawnPoints[spawnPointIdx++];
			Spheroid_Obj->PlaySpawnAnim = true;
			Spheroid_Obj->NextWaypoint = GenerateRandomWaypoint(Spheroid_Obj->Bounds);
			Spheroid_Obj->FlyElapsedTime = 0.0f;
			Spheroid_Obj->FlyDuration = randGetFloat(SPHEROID_MIN_FLY_DURATION, SPHEROID_MAX_FLY_DURATION);
			Spheroid_Obj->EnforcerSpawnElapsedTime = 0.0f;
			Spheroid_Obj->EnforcerSpawnDelay = randGetFloat(SPHEROID_MIN_ENFORCER_SPAWN_DELAY, SPHEROID_MAX_ENFORCER_SPAWN_DELAY);
			float randVel = randGetFloat(SPHEROID_MIN_VELOCITY, SPHEROID_MAX_VELOCITY);
			Spheroid_Obj->Obj.velocity = (Coord2D){ randVel , randVel };
		}
	}
}

void EnemyManagerRecycleSpheroid(EnemyManager* Manager, Spheroid* Spheroid_Obj)
{
	assert(Manager != NULL && Spheroid_Obj != NULL);

	// Deactivate the spheroid object
	Spheroid_Obj->Obj.IsActive = false;
	Spheroid_Obj->FlyElapsedTime = 0.0f;
	Spheroid_Obj->EnforcerSpawnElapsedTime = 0.0f;
	--Manager->ActiveEnemies[2];
	--Manager->TotalEnemies;

	if (Manager->TotalEnemies == 0)
		_enemyManagerTriggerAllEnemiesDeadCB();
}

void EnemyManagerRecycleAllSpheroids(EnemyManager* Manager)
{
	assert(Manager != NULL);

	// Deactivate the spheroid objects
	for (int i = 0; i < MAX_SPHEROIDS; i++)
	{
		Spheroid* Spheroid_Obj = Manager->Spheroids[i];
		assert(Spheroid_Obj != NULL);

		if (Spheroid_Obj->Obj.IsActive == true)
		{
			Spheroid_Obj->Obj.IsActive = false;
			--Manager->ActiveEnemies[2];
		}
	}
}

void EnemyManagerSpawnQuark(EnemyManager* Manager, Coord2D Position)
{
	assert(Manager != NULL);

	// Prevent spawning if max quarks are active
	if (Manager->ActiveEnemies[3] >= MAX_QUARKS)
	{
		return;
	}

	for (int i = 0; i < MAX_QUARKS; i++)
	{
		Quark* Quark_Obj = Manager->Quarks[i];
		assert(Quark_Obj != NULL);

		if (Quark_Obj->Obj.IsActive == false)
		{
			Quark_Obj->Obj.position = Position;
			Quark_Obj->Obj.IsActive = true;
			Quark_Obj->NextWaypoint = GenerateRandomWaypoint(Quark_Obj->Bounds);
			Quark_Obj->FlyElapsedTime = 0.0f;
			Quark_Obj->FlyDuration = randGetFloat(QUARK_MIN_FLY_DURATION, QUARK_MAX_FLY_DURATION);
			Quark_Obj->TankSpawnElapsedTime = 0.0f;
			Quark_Obj->TankSpawnDelay = randGetFloat(QUARK_MIN_TANK_SPAWN_DELAY, QUARK_MAX_TANK_SPAWN_DELAY);
			float randVel = randGetFloat(QUARK_MIN_VELOCITY, QUARK_MAX_VELOCITY);
			Quark_Obj->Obj.velocity = (Coord2D){ randVel , randVel };
			++Manager->ActiveEnemies[3];
			++Manager->TotalEnemies;

			return;
		}
	}
}

void EnemyManagerRespawnQuarks(EnemyManager* Manager, Coord2D* RandSpawnPoints)
{
	assert(Manager != NULL && RandSpawnPoints != NULL);

	int spawnPointIdx = 0;
	for (int i = 0; i < MAX_QUARKS; i++)
	{
		Quark* Quark_Obj = Manager->Quarks[i];
		assert(Quark_Obj != NULL);

		if (Quark_Obj->Obj.IsActive == true)
		{
			Quark_Obj->Obj.position = RandSpawnPoints[spawnPointIdx++];
			Quark_Obj->NextWaypoint = GenerateRandomWaypoint(Quark_Obj->Bounds);
			Quark_Obj->FlyElapsedTime = 0.0f;
			Quark_Obj->FlyDuration = randGetFloat(QUARK_MIN_FLY_DURATION, QUARK_MAX_FLY_DURATION);
			Quark_Obj->TankSpawnElapsedTime = 0.0f;
			Quark_Obj->TankSpawnDelay = randGetFloat(QUARK_MIN_TANK_SPAWN_DELAY, QUARK_MAX_TANK_SPAWN_DELAY);
			float randVel = randGetFloat(QUARK_MIN_VELOCITY, QUARK_MAX_VELOCITY);
			Quark_Obj->Obj.velocity = (Coord2D){ randVel , randVel };
		}
	}
}

void EnemyManagerRecycleQuark(EnemyManager* Manager, Quark* Quark_Obj)
{
	assert(Manager != NULL && Quark_Obj != NULL);

	// Deactivate the quark object
	Quark_Obj->Obj.IsActive = false;
	Quark_Obj->FlyElapsedTime = 0.0f;
	Quark_Obj->TankSpawnElapsedTime = 0.0f;
	--Manager->ActiveEnemies[3];
	--Manager->TotalEnemies;

	if (Manager->TotalEnemies == 0)
		_enemyManagerTriggerAllEnemiesDeadCB();
}

void EnemyManagerRecycleAllQuarks(EnemyManager* Manager)
{
	assert(Manager != NULL);

	// Deactivate the quark objects
	for (int i = 0; i < MAX_QUARKS; i++)
	{
		Quark* Quark_Obj = Manager->Quarks[i];
		assert(Quark_Obj != NULL);

		if (Quark_Obj->Obj.IsActive == true)
		{
			Quark_Obj->Obj.IsActive = false;
			--Manager->ActiveEnemies[3];
		}
	}
}

void RecycleAllEnemies(EnemyManager* Manager)
{
	assert(Manager != NULL);

	EnemyManagerRecycleAllGrunts(Manager);
	EnemyManagerRecycleAllHulks(Manager);
	EnemyManagerRecycleAllSpheroids(Manager);
	EnemyManagerRecycleAllQuarks(Manager);

	Manager->TotalEnemies = 0;
}

void EnemyManagerDestroy(EnemyManager* Manager)
{
	assert(Manager != NULL);

	// Delete every grunt
	for (int i = 0; i < MAX_GRUNTS; ++i) {
		GruntDelete(Manager->Grunts[i]);
		Manager->Grunts[i] = NULL;
	}
	Manager->ActiveEnemies[0] = 0;

	// Delete every hulk
	for (int i = 0; i < MAX_HULKS; ++i) {
		HulkDelete(Manager->Hulks[i]);
		Manager->Hulks[i] = NULL;
	}
	Manager->ActiveEnemies[1] = 0;

	// Delete every spheroid
	for (int i = 0; i < MAX_SPHEROIDS; ++i) {
		SpheroidDelete(Manager->Spheroids[i]);
		Manager->Spheroids[i] = NULL;
	}
	Manager->ActiveEnemies[2] = 0;

	// Delete every quark
	for (int i = 0; i < MAX_QUARKS; ++i) {
		QuarkDelete(Manager->Quarks[i]);
		Manager->Quarks[i] = NULL;
	}
	Manager->ActiveEnemies[3] = 0;

	Manager->TotalEnemies = 0;

	free(Manager);
	Manager = NULL;
}

void _enemyManagerTriggerAllEnemiesDeadCB()
{
	if (_allEnemiesKilledCB)
	{
		_allEnemiesKilledCB();
	}
}