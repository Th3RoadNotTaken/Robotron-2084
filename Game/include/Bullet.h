#pragma once
#include "baseTypes.h"
#include "Object.h"

// Object Properties
#define BULLET_THICKNESS   3
#define BULLET_SPEED	   0.7f
#define DIAGONAL_SPEED     0.7071f	// 1/sqrt(2) for normalizing magnitude
#define BULLET_LENGTH	   10.0f
#define DEFAULT_COLOR	   0xFFFF0000 
#define COLOR_CHANGE_DELAY 100

#define COLLISION_TYPES    8

typedef struct Bullet_t {
	Object Obj;

	Bounds2D Bounds;
	float Length;
} Bullet;

typedef void (*BulletCollideCB)(Object*);
void BulletSetCollideCB(BulletCollideCB cb, enum ObjectType objtype);
void BulletClearCollideCB(enum ObjectType objtype);

Bounds2D CalculateBulletBounds(Coord2D StartPosition, enum Direction Dir);
Bullet* BulletNew(Coord2D Position, Bounds2D FieldBounds, enum Direction Dir, bool IsActive);
void BulletDelete(Bullet* Bullet_Obj);