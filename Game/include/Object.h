#pragma once
#include "baseTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_COLOR              0xFFFF0000 
#define DEFAULT_COLOR_CHANGE_DELAY 200

// object "virtual" functions
typedef struct object_t Object;
typedef void (*ObjDrawFunc)(Object*);
typedef void (*ObjUpdateFunc)(Object*, uint32_t);
typedef void (*ObjCollisionFunc)(Object*, Object*, enum ObjectType, enum ObjectType);

typedef struct object_vtable_t {
    ObjDrawFunc     draw;
    ObjUpdateFunc   update;
} ObjVtable;

typedef struct object_collision_vtable_t {
    ObjCollisionFunc    coll;
} ObjCollisionVtable;

typedef struct object_t {
    ObjVtable*           vtable;
    Coord2D              position;
    Coord2D              velocity;
    bool                 IsActive;
    bool                 IsPaused;
    bool                 CanSpawn;
    Bounds2D             objBounds;
    enum Direction       direction;
    uint32_t             color;
    uint32_t             colorUpdateElapsedTime;
    uint32_t             colorChangeDelay;
    enum ObjectType      objtype;
    enum ObjectSubType   objSubType;
    ObjCollisionVtable*  collVtable;

} Object;

typedef void (*ObjRegistrationFunc)(Object*);

// class-wide registration methods
void objEnableRegistration(ObjRegistrationFunc registerFunc, ObjRegistrationFunc deregisterFunc);
void objDisableRegistration();

// object API
void objInit(Object* obj, ObjVtable* vtable, Coord2D pos, Coord2D vel, Bounds2D objBounds, enum Direction dir, uint32_t color, uint32_t colorChangeDelay, enum ObjectType objtype, ObjCollisionVtable* collVTable);
void objInitDefault(Object* obj, ObjVtable* vtable, Coord2D pos, Coord2D vel, enum ObjectType objtype, ObjCollisionVtable* collVTable);
void objDeinit(Object* obj);
void objDraw(Object* obj);
void objUpdate(Object* obj, uint32_t milliseconds);
void objCollTest(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType);
void objSetRandomColor(Object* obj, uint32_t milliseconds);
Coord2D GenerateRandomWaypoint(Bounds2D FieldBounds);

// default update implementation that just moves at the current velocity
void objDefaultUpdate(Object* obj, uint32_t milliseconds);

#ifdef __cplusplus
}
#endif