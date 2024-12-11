#include <stdlib.h>

#include "baseTypes.h"
#include "shape.h"
#include "ball.h"
#include "random.h"
#include "field.h"
#include "Object.h"

typedef struct ball_t {
	Object obj;

	Bounds2D bounds;
	uint32_t color;
	float radius;
} Ball;

// the object vtable for all balls
static void _ballUpdate(Object* obj, uint32_t milliseconds);
static void _ballDraw(Object* obj);
static void _ballColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType);
static ObjVtable _ballVtable = {
	_ballDraw,
	_ballUpdate
};
static ObjCollisionVtable _ballCollVTable = {
	_ballColl
};

// storage for a collision callback
static BallCollideCB _ballCollideCB = NULL;

// other private methods
static void _ballSetRandomColor(Ball* ball);
static void _ballDoCollisions(Ball* ball);
static void _ballCollideField(Ball* ball);
static void _ballTriggerCollideCB(Ball* ball);

/// @brief Sets the callback
/// @param cb 
void ballSetCollideCB(BallCollideCB cb)
{
	_ballCollideCB = cb;
}

/// @brief Clears the callback
void ballClearCollideCB()
{
	_ballCollideCB = NULL;
}

/// @brief Instantiate and initialize a ball object
/// @param bounds 
/// @return 
Ball* ballNew(Bounds2D bounds)
{
	const float MAX_VEL = 5.0f;
	const float MIN_RADIUS = 10.0f;
	const float MAX_RADIUS = 50.0f;

	Ball* ball = malloc(sizeof(Ball));
	if (ball != NULL)
	{
		Coord2D pos = boundsGetCenter(&bounds);
		Coord2D vel = { randGetFloat(-MAX_VEL, MAX_VEL), randGetFloat(-MAX_VEL, MAX_VEL) };
		objInitDefault(&ball->obj, &_ballVtable, pos, vel, DefaultObjType, &_ballCollVTable);

		ball->bounds = bounds;
		ball->radius = randGetFloat(MIN_RADIUS, MAX_RADIUS);
		_ballSetRandomColor(ball);
	}
	return ball;
}

/// @brief Free up any resources pertaining to a ball object
/// @param ball 
void ballDelete(Ball* ball)
{
	objDeinit(&ball->obj);

	free(ball);
}

/// @brief Sets the color of the ball to a random RGB
/// @param ball 
static void _ballSetRandomColor(Ball* ball)
{
	ball->color = randGetInt(0, 256);
	ball->color += randGetInt(0, 256) << 8;
	ball->color += randGetInt(0, 256) << 16;
}

/// @brief Mainly process ball collisions
/// @param obj 
/// @param milliseconds 
static void _ballUpdate(Object* obj, uint32_t milliseconds)
{
	objDefaultUpdate(obj, milliseconds);

	_ballDoCollisions((Ball*)obj);
}

static void _ballDraw(Object* obj)
{
	Ball* ball = (Ball*)obj;

	uint8_t red = (uint8_t)((ball->color >> 16) & 0xFF);
	uint8_t green = (uint8_t)((ball->color >> 8) & 0xFF);
	uint8_t blue = (uint8_t)((ball->color >> 0) & 0xFF);
	bool filledVal = true;

	shapeDrawCircle(ball->radius, ball->obj.position.x, ball->obj.position.y, red, green, blue, filledVal);
}

void _ballColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType)
{
}

static void _ballDoCollisions(Ball* ball)
{
	_ballCollideField(ball);
}

static void _ballCollideField(Ball* ball)
{
	float rightSide = ball->bounds.botRight.x;
	float leftSide = ball->bounds.topLeft.x;
	float topSide = ball->bounds.topLeft.y;
	float bottomSide = ball->bounds.botRight.y;

	if(ball->obj.position.x - ball->radius <= leftSide  )
	{
		ball->obj.velocity.x = -ball->obj.velocity.x;
		ball->obj.position.x = leftSide + ball->radius;
		_ballSetRandomColor(ball);
		_ballTriggerCollideCB(ball);
	}
	if(ball->obj.position.x + ball->radius >= rightSide  )
	{
		ball->obj.velocity.x = -ball->obj.velocity.x;
		ball->obj.position.x = rightSide - ball->radius;
		_ballSetRandomColor(ball);
		_ballTriggerCollideCB(ball);
	}
	if(ball->obj.position.y + ball->radius >= bottomSide  )
	{
		ball->obj.velocity.y = -ball->obj.velocity.y;
		ball->obj.position.y = bottomSide - ball->radius;
		_ballSetRandomColor(ball);
		_ballTriggerCollideCB(ball);
	}
	if(ball->obj.position.y - ball->radius <= topSide  )
	{
		ball->obj.velocity.y = -ball->obj.velocity.y;
		ball->obj.position.y = topSide + ball->radius;
		_ballSetRandomColor(ball);
		_ballTriggerCollideCB(ball);
	}
}

/// @brief Triggers the callback, if one is registered
/// @param ball 
static void _ballTriggerCollideCB(Ball* ball)
{
	if (_ballCollideCB != NULL)
	{
		_ballCollideCB(ball);
	}
}

