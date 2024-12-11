#pragma once

#include "Bullet.h"

#define MAX_BULLETS 50
#define FIRE_TIMER  120

typedef struct BulletManager_t BulletManager;

BulletManager* BulletManagerInit(Bounds2D FieldBounds);
void BulletManagerSpawnBullet(BulletManager* Manager, Coord2D Position, enum Direction Dir, uint32_t DeltaTime);
void BulletManagerRecycleBullet(BulletManager* Manager, Bullet* bullet);
void BulletManagerRecycleAllBullets(BulletManager* Manager);
void BulletManagerDestroy(BulletManager* Manager);