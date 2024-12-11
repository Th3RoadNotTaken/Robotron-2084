#include "baseTypes.h"
#include "object.h"
#include "assert.h"
#include "random.h"

static ObjRegistrationFunc _registerFunc = NULL;
static ObjRegistrationFunc _deregisterFunc = NULL;

/// @brief Enable callback to a registrar on ObjInit/Deinit
/// @param registerFunc 
/// @param deregisterFunc 
void objEnableRegistration(ObjRegistrationFunc registerFunc, ObjRegistrationFunc deregisterFunc)
{
    _registerFunc = registerFunc;
    _deregisterFunc = deregisterFunc;
}

/// @brief Disable registration during ObjInit/Deinit
void objDisableRegistration()
{
    _registerFunc = _deregisterFunc = NULL;
}

/// @brief Initialize an object. Intended to be called from subclass constructors
/// @param obj 
/// @param vtable 
/// @param pos 
/// @param vel 
void objInit(Object* obj, ObjVtable* vtable, Coord2D pos, Coord2D vel, Bounds2D objBounds, enum Direction dir, uint32_t color, uint32_t colorChangeDelay, enum ObjectType objtype, ObjCollisionVtable* collVTable)
{
    obj->vtable = vtable;
    obj->position = pos;
    obj->velocity = vel;
    obj->objBounds = objBounds;
    obj->direction = dir;
    obj->color = color;
    obj->IsActive = true;
    obj->IsPaused = false;
    obj->CanSpawn = true;
    obj->colorUpdateElapsedTime = 0;
    obj->colorChangeDelay = colorChangeDelay;
    obj->objtype = objtype;
    obj->collVtable = collVTable;

    if (_registerFunc != NULL)
    {
        _registerFunc(obj);
    }
}

void objInitDefault(Object* obj, ObjVtable* vtable, Coord2D pos, Coord2D vel, enum ObjectType objtype, ObjCollisionVtable* collVTable)
{
    objInit(obj, vtable, pos, vel, (Bounds2D) { { 0, 0 }, { 0,0 } }, DefaultDir, DEFAULT_COLOR, DEFAULT_COLOR_CHANGE_DELAY, objtype, collVTable);
}

/// @brief Deinitialize an object
/// @param obj 
void objDeinit(Object* obj)
{
    if (_deregisterFunc != NULL)
    {
        _deregisterFunc(obj);
    }
}

/// @brief Draw this object, using it's vtable
/// @param obj 
void objDraw(Object* obj)
{
    if (obj->vtable != NULL && obj->vtable->draw != NULL) 
    {
        obj->vtable->draw(obj);
    }
}

/// @brief Update this object, using it's vtable
/// @param obj 
/// @param milliseconds 
void objUpdate(Object* obj, uint32_t milliseconds)
{
    if (obj->vtable != NULL && obj->vtable->update != NULL) 
    {
        obj->vtable->update(obj, milliseconds);
        return;
    }
    objDefaultUpdate(obj, milliseconds);
}

// Calls the collision function pointers for every object type pair that is supposed to collide
void objCollTest(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType)
{
    if (SourceObject == NULL || TargetObject == NULL || SourceObject == TargetObject) return;

    if (SourceObject->collVtable != NULL && SourceObject->collVtable->coll != NULL &&
        TargetObject->collVtable != NULL && TargetObject->collVtable->coll != NULL)
    {
        SourceObject->collVtable->coll(SourceObject, TargetObject, SourceObjType, TargetObjectType);
        TargetObject->collVtable->coll(TargetObject, SourceObject, TargetObjectType, SourceObjType);
    }
}

void objDefaultUpdate(Object* obj, uint32_t milliseconds)
{
    obj->position.x += obj->velocity.x;
    obj->position.y += obj->velocity.y;
}

// Sets random rgb values for an object each frame
void objSetRandomColor(Object* obj, uint32_t milliseconds)
{
    assert(obj != NULL);

    obj->colorUpdateElapsedTime += milliseconds;

    if (obj->colorUpdateElapsedTime < obj->colorChangeDelay)
        return;
    else
        obj->colorUpdateElapsedTime = 0;

    obj->color = randGetInt(0, 256);
    obj->color += randGetInt(0, 256) << 8;
    obj->color += randGetInt(0, 256) << 16;
}

// Generates a random waypoint inside the game's field
Coord2D GenerateRandomWaypoint(Bounds2D FieldBounds)
{
    Coord2D waypoint;
    waypoint.x = randGetFloat(FieldBounds.topLeft.x, FieldBounds.botRight.x);
    waypoint.y = randGetFloat(FieldBounds.topLeft.y, FieldBounds.botRight.y);
    return waypoint;
}