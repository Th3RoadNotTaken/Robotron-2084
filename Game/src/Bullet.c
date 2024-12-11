#include <stdlib.h>

#include "baseTypes.h"
#include "shape.h"
#include "random.h"
#include "field.h"
#include "Bullet.h"
#include "assert.h"
#include "stdbool.h"
#include "math.h"

// the object vtable for bullets
static void BulletUpdate(Object* obj, uint32_t milliseconds);
static void BulletDraw(Object* obj);
static void BulletColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType);

static ObjVtable BulletVtable = {
	BulletDraw,
	BulletUpdate
};

static ObjCollisionVtable BulletCollVTable = {
	BulletColl
};

// storage for a collision callback
static BulletCollideCB _bulletCollideCB[COLLISION_TYPES] = { NULL };

static void BulletDoCollisions(Bullet* Bullet_Obj);
static void BulletTriggerCollideCB(Object* Obj, enum ObjectType objtype);

void BulletSetCollideCB(BulletCollideCB cb, enum ObjectType objtype)
{
	switch (objtype)
	{
	case DefaultObjType: _bulletCollideCB[0] = cb; break;
	case EnemyGrunt: _bulletCollideCB[1] = cb; break;
	case EnemyHulk: _bulletCollideCB[2] = cb; break;
	case EnemySpheroid: _bulletCollideCB[3] = cb; break;
	case EnemyEnforcer: _bulletCollideCB[4] = cb; break;
	case Obstacle: _bulletCollideCB[5] = cb; break;
	case EnemyQuark: _bulletCollideCB[6] = cb; break;
	case EnemyTank: _bulletCollideCB[7] = cb; break;
	}
}

void BulletClearCollideCB(enum ObjectType objtype)
{
	switch (objtype)
	{
	case DefaultObjType: _bulletCollideCB[0] = NULL; break;
	case EnemyGrunt: _bulletCollideCB[1] = NULL; break;
	case EnemyHulk: _bulletCollideCB[2] = NULL; break;
	case EnemySpheroid: _bulletCollideCB[3] = NULL; break;
	case EnemyEnforcer: _bulletCollideCB[4] = NULL; break;
	case Obstacle: _bulletCollideCB[5] = NULL; break;
	case EnemyQuark: _bulletCollideCB[6] = NULL; break;
	case EnemyTank: _bulletCollideCB[7] = NULL; break;
	}
}

Bounds2D CalculateBulletBounds(Coord2D StartPosition, enum Direction Dir)
{
	Coord2D EndPosition;
	if (Dir == Up || Dir == Down || Dir == Left || Dir == Right)
	{
		EndPosition.x = StartPosition.x + BULLET_LENGTH;
		EndPosition.y = StartPosition.y + BULLET_LENGTH;
	}
	else
	{
		// Calculate the end point of the line using sin(45) and cos(45)
		EndPosition.x = StartPosition.x + BULLET_LENGTH * DIAGONAL_SPEED;
		EndPosition.y = StartPosition.y + BULLET_LENGTH * DIAGONAL_SPEED;
	}

	return (Bounds2D) { { StartPosition.x, StartPosition.y }, { EndPosition.x, EndPosition.y } };
}

Bullet* BulletNew(Coord2D Position, Bounds2D FieldBounds, enum Direction Dir, bool IsActive)
{
	Bullet* Bullet_Obj = malloc(sizeof(Bullet));

	if (Bullet_Obj != NULL)
	{
		Coord2D Vel = { BULLET_SPEED, BULLET_SPEED };
		objInit(&Bullet_Obj->Obj, &BulletVtable, Position, Vel, CalculateBulletBounds(Position, Dir), Dir, DEFAULT_COLOR, COLOR_CHANGE_DELAY, Projectile, &BulletCollVTable);

		Bullet_Obj->Bounds = FieldBounds;
		Bullet_Obj->Length = BULLET_LENGTH;
		Bullet_Obj->Obj.IsActive = IsActive;
	}

	return Bullet_Obj;
}

void BulletDelete(Bullet* Bullet_Obj)
{
	objDeinit(&Bullet_Obj->Obj);

	free(Bullet_Obj);
}

void BulletDraw(Object* obj)
{
	assert(obj != NULL);
	Bullet* Bullet_Obj = (Bullet*)obj;
	assert(Bullet_Obj != NULL);

	if (obj->IsActive == false)
		return;

	uint8_t Red = (uint8_t)((obj->color >> 16) & 0xFF);
	uint8_t Green = (uint8_t)((obj->color >> 8) & 0xFF);
	uint8_t Blue = (uint8_t)((obj->color >> 0) & 0xFF);
	bool filledVal = true;

	switch (obj->direction)
	{
	case Up: shapeDrawLine(obj->position.x, obj->position.y, obj->position.x, obj->position.y - BULLET_LENGTH, Red, Green, Blue, BULLET_THICKNESS); break;
	case Down: shapeDrawLine(obj->position.x, obj->position.y, obj->position.x, obj->position.y + BULLET_LENGTH, Red, Green, Blue, BULLET_THICKNESS); break;
	case Left: shapeDrawLine(obj->position.x, obj->position.y, obj->position.x - BULLET_LENGTH, obj->position.y, Red, Green, Blue, BULLET_THICKNESS); break;
	case Right: shapeDrawLine(obj->position.x, obj->position.y, obj->position.x + BULLET_LENGTH, obj->position.y, Red, Green, Blue, BULLET_THICKNESS); break;
	case UpLeft: shapeDrawLine(obj->position.x, obj->position.y, obj->position.x - BULLET_LENGTH * DIAGONAL_SPEED, obj->position.y - BULLET_LENGTH * DIAGONAL_SPEED, Red, Green, Blue, BULLET_THICKNESS); break;
	case UpRight: shapeDrawLine(obj->position.x, obj->position.y, obj->position.x + BULLET_LENGTH * DIAGONAL_SPEED, obj->position.y - BULLET_LENGTH * DIAGONAL_SPEED, Red, Green, Blue, BULLET_THICKNESS); break;
	case DownLeft: shapeDrawLine(obj->position.x, obj->position.y, obj->position.x - BULLET_LENGTH * DIAGONAL_SPEED, obj->position.y + BULLET_LENGTH * DIAGONAL_SPEED, Red, Green, Blue, BULLET_THICKNESS); break;
	case DownRight: shapeDrawLine(obj->position.x, obj->position.y, obj->position.x + BULLET_LENGTH * DIAGONAL_SPEED, obj->position.y + BULLET_LENGTH * DIAGONAL_SPEED, Red, Green, Blue, BULLET_THICKNESS); break;
	default: shapeDrawLine(obj->position.x, obj->position.y, obj->position.x, obj->position.y + BULLET_LENGTH, Red, Green, Blue, BULLET_THICKNESS); break; // Default to down if none
	}
}

void BulletUpdate(Object* obj, uint32_t milliseconds)
{
	assert(obj != NULL);
	Bullet* Bullet_Obj = (Bullet*)obj;
	assert(Bullet_Obj != NULL);

	if (obj->IsActive == false || obj->IsPaused == true)
		return;

	objSetRandomColor(obj, milliseconds);

	BulletDoCollisions(Bullet_Obj);

	switch (obj->direction)
	{
	case Up:
		obj->position.y -= obj->velocity.y * milliseconds;
		break;
	case Down:
		obj->position.y += obj->velocity.y * milliseconds;
		break;
	case Left:
		obj->position.x -= obj->velocity.x * milliseconds;
		break;
	case Right:
		obj->position.x += obj->velocity.x * milliseconds;
		break;
	case UpLeft:
		obj->position.y -= obj->velocity.y * DIAGONAL_SPEED * milliseconds;
		obj->position.x -= obj->velocity.x * DIAGONAL_SPEED * milliseconds;
		break;
	case UpRight:
		obj->position.y -= obj->velocity.y * DIAGONAL_SPEED * milliseconds;
		obj->position.x += obj->velocity.x * DIAGONAL_SPEED * milliseconds;
		break;
	case DownLeft:
		obj->position.y += obj->velocity.y * DIAGONAL_SPEED * milliseconds;
		obj->position.x -= obj->velocity.x * DIAGONAL_SPEED * milliseconds;
		break;
	case DownRight:
		obj->position.y += obj->velocity.y * DIAGONAL_SPEED * milliseconds;
		obj->position.x += obj->velocity.x * DIAGONAL_SPEED * milliseconds;
		break;
	default:
		break;
	}

	// Calculate bounds every frame
	obj->objBounds = CalculateBulletBounds(obj->position, obj->direction);
}

void BulletDoCollisions(Bullet* Bullet_Obj)
{
	float rightSide = Bullet_Obj->Bounds.botRight.x;
	float leftSide = Bullet_Obj->Bounds.topLeft.x;
	float topSide = Bullet_Obj->Bounds.topLeft.y;
	float bottomSide = Bullet_Obj->Bounds.botRight.y;

	if (Bullet_Obj->Obj.position.x <= leftSide ||
		Bullet_Obj->Obj.position.x >= rightSide ||
		Bullet_Obj->Obj.position.y >= bottomSide ||
		Bullet_Obj->Obj.position.y <= topSide)
	{
		BulletTriggerCollideCB(&Bullet_Obj->Obj, DefaultObjType);
	}
}

static void BulletTriggerCollideCB(Object* Obj, enum ObjectType objtype)
{
	if (Obj == NULL) return;

	switch (objtype)
	{
	case DefaultObjType: _bulletCollideCB[0](Obj); break;
	case EnemyGrunt: _bulletCollideCB[1](Obj); break;
	case EnemyHulk: _bulletCollideCB[2](Obj); break;
	case EnemySpheroid: _bulletCollideCB[3](Obj); break;
	case EnemyEnforcer: _bulletCollideCB[4](Obj); break;
	case Obstacle: _bulletCollideCB[5](Obj); break;
	case EnemyQuark: _bulletCollideCB[6](Obj); break;
	case EnemyTank: _bulletCollideCB[7](Obj); break;
	}
}

void BulletColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType)
{
	assert(SourceObject != NULL && TargetObject != NULL);

	if (SourceObject->IsActive == false || TargetObject->IsActive == false) return;

	Bullet* Bullet_Obj = (Bullet*)SourceObject;
	assert(Bullet_Obj != NULL);

	float TargetRightSide = TargetObject->objBounds.botRight.x;
	float TargetLeftSide = TargetObject->objBounds.topLeft.x;
	float TargetTopSide = TargetObject->objBounds.topLeft.y;
	float TargetBottomSide = TargetObject->objBounds.botRight.y;

	Coord2D SourceStartingPoint = SourceObject->objBounds.topLeft;
	Coord2D SourceEndPoint = SourceObject->objBounds.botRight;

	// Check if bullet line starting point is within the target collision box
	if (SourceStartingPoint.x >= TargetLeftSide && SourceStartingPoint.x <= TargetRightSide &&
		SourceStartingPoint.y >= TargetTopSide && SourceStartingPoint.y <= TargetBottomSide)
	{
		if (TargetObjectType == EnemyGrunt)
		{
			BulletTriggerCollideCB(TargetObject, EnemyGrunt);
			BulletTriggerCollideCB(SourceObject, DefaultObjType);
		}
		else if (TargetObjectType == EnemyHulk)
		{
			BulletTriggerCollideCB(TargetObject, EnemyHulk);
			BulletTriggerCollideCB(SourceObject, DefaultObjType);
		}
		else if (TargetObjectType == EnemySpheroid)
		{
			BulletTriggerCollideCB(TargetObject, EnemySpheroid);
			BulletTriggerCollideCB(SourceObject, DefaultObjType);
		}
		else if (TargetObjectType == EnemyEnforcer)
		{
			BulletTriggerCollideCB(TargetObject, EnemyEnforcer);
			BulletTriggerCollideCB(SourceObject, DefaultObjType);
		}
		else if (TargetObjectType == Obstacle)
		{
			BulletTriggerCollideCB(TargetObject, Obstacle);
			BulletTriggerCollideCB(SourceObject, DefaultObjType);
		}
		else if (TargetObjectType == EnemyQuark)
		{
			BulletTriggerCollideCB(TargetObject, EnemyQuark);
			BulletTriggerCollideCB(SourceObject, DefaultObjType);
		}
		else if (TargetObjectType == EnemyTank)
		{
			BulletTriggerCollideCB(TargetObject, EnemyTank);
			BulletTriggerCollideCB(SourceObject, DefaultObjType);
		}
		return;
	}

	// Check if bullet line end point is within the target collision box
	if (SourceEndPoint.x >= TargetLeftSide && SourceEndPoint.x <= TargetRightSide &&
		SourceEndPoint.y >= TargetTopSide && SourceEndPoint.y <= TargetBottomSide)
	{
		if (TargetObjectType == EnemyGrunt)
		{
			BulletTriggerCollideCB(TargetObject, EnemyGrunt);
			BulletTriggerCollideCB(SourceObject, DefaultObjType);
		}
		else if (TargetObjectType == EnemyHulk)
		{
			BulletTriggerCollideCB(TargetObject, EnemyHulk);
			BulletTriggerCollideCB(SourceObject, DefaultObjType);
		}
		else if (TargetObjectType == EnemySpheroid)
		{
			BulletTriggerCollideCB(TargetObject, EnemySpheroid);
			BulletTriggerCollideCB(SourceObject, DefaultObjType);
		}
		else if (TargetObjectType == EnemyEnforcer)
		{
			BulletTriggerCollideCB(TargetObject, EnemyEnforcer);
			BulletTriggerCollideCB(SourceObject, DefaultObjType);
		}
		else if (TargetObjectType == Obstacle)
		{
			BulletTriggerCollideCB(TargetObject, Obstacle);
			BulletTriggerCollideCB(SourceObject, DefaultObjType);
		}
		else if (TargetObjectType == EnemyQuark)
		{
			BulletTriggerCollideCB(TargetObject, EnemyQuark);
			BulletTriggerCollideCB(SourceObject, DefaultObjType);
		}
		else if (TargetObjectType == EnemyTank)
		{
			BulletTriggerCollideCB(TargetObject, EnemyTank);
			BulletTriggerCollideCB(SourceObject, DefaultObjType);
		}
	}
}