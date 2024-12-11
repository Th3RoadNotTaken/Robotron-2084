#include <stdlib.h>
#include "assert.h"
#include "BulletManager.h"
#include "Object.h"
#include "baseTypes.h"
#include "sound.h"

typedef struct BulletManager_t
{
	Bullet* Bullets[MAX_BULLETS];
	uint8_t ActiveBulletCount;
	uint32_t ElapsedTime;
} BulletManager;

static int32_t _bulletSoundId = SOUND_NOSOUND;

// Initializes the bullet pool 
BulletManager* BulletManagerInit(Bounds2D FieldBounds)
{
	BulletManager* Manager = malloc(sizeof(BulletManager));
	assert(Manager != NULL);

	for (int i = 0; i < MAX_BULLETS; i++)
	{
		Manager->Bullets[i] = BulletNew((Coord2D) { 0.0f, 0.0f }, FieldBounds, Down, false);
	}
	Manager->ActiveBulletCount = 0;
	Manager->ElapsedTime = 0;

	_bulletSoundId = soundLoad("asset/sounds/BulletFire.wav");

	return Manager;
}

// Spawns an available bullet from the pool
void BulletManagerSpawnBullet(BulletManager* Manager, Coord2D Position, enum Direction Dir, uint32_t DeltaTime)
{
	assert(Manager != NULL);

	// Prevent spawning if max bullets are active
	if (Manager->ActiveBulletCount >= MAX_BULLETS)
	{
		return;
	}

	Manager->ElapsedTime += DeltaTime;
	if (Manager->ElapsedTime < FIRE_TIMER)
	{
		return;
	}
	else
	{
		Manager->ElapsedTime = 0;
	}

	for (int i = 0; i < MAX_BULLETS; i++)
	{
		Bullet* Bullet_Obj = Manager->Bullets[i];
		assert(Bullet_Obj != NULL);

		if (Bullet_Obj->Obj.IsActive == false)
		{
			Bullet_Obj->Obj.position = Position;
			Bullet_Obj->Obj.direction = Dir;
			Bullet_Obj->Obj.IsActive = true;
			++Manager->ActiveBulletCount;

			soundPlay(_bulletSoundId);

			return;
		}
	}
}

// Recycles the bullet back into the pool
void BulletManagerRecycleBullet(BulletManager* Manager, Bullet* bullet)
{
	assert(Manager != NULL && bullet != NULL);

	// Deactivate the bullet
	bullet->Obj.IsActive = false;
	--Manager->ActiveBulletCount;
}

// Recycles all active bullets back into the pool
void BulletManagerRecycleAllBullets(BulletManager* Manager)
{
	assert(Manager != NULL);

	for (int i = 0; i < MAX_BULLETS; i++)
	{
		Bullet* Bullet_Obj = Manager->Bullets[i];
		assert(Bullet_Obj != NULL);

		if (Bullet_Obj->Obj.IsActive == true)
		{
			Bullet_Obj->Obj.IsActive = false;
			--Manager->ActiveBulletCount;
		}
	}
}

// Destroys the pool and all associated objects
void BulletManagerDestroy(BulletManager* Manager)
{
	assert(Manager != NULL);

	for (int i = 0; i < MAX_BULLETS; ++i) {
		BulletDelete(Manager->Bullets[i]);
		Manager->Bullets[i] = NULL;
	}
	Manager->ActiveBulletCount = 0;

	soundUnload(_bulletSoundId);

	free(Manager);
	Manager = NULL;
}