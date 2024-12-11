#include <Windows.h>
#include <stdlib.h>
#include <assert.h>
#include "objmgr.h"
#include "baseTypes.h"
#include "stdbool.h"

static struct objmgr_t {
    Object** list;
    uint32_t max;
    uint32_t count;
} _objMgr = { NULL, 0, 0 };

// Timer
static float pauseElapsedTime = 0.0f;
static float pauseDuration = 3000.0f;
static float resumeElapsedTime = 0.0f;
static float resumeDuration = 3000.0f;

static bool drawPause = false;

/// @brief Initialize the object manager
/// @param maxObjects 
void objMgrInit(uint32_t maxObjects)
{
    // allocate the required space
    _objMgr.list = malloc(maxObjects * sizeof(Object*));
    if (_objMgr.list != NULL) {
        // initialize as empty
        ZeroMemory(_objMgr.list, maxObjects * sizeof(Object*));
        _objMgr.max = maxObjects;
        _objMgr.count = 0;
    }

    // setup registration, so all initialized objects are logged w/ the manager
    objEnableRegistration(objMgrAdd, objMgrRemove);
}

/// @brief Shutdown the object manager
void objMgrShutdown()
{
    for (uint32_t i = 0; i < _objMgr.max; ++i)
    {
        if (_objMgr.list[i] != NULL)
        {
            continue;
        }
    }
    // disable registration, since the object manager is shutting down
    objDisableRegistration();

    // this isn't strictly required, but want to enforce proper cleanup
    assert(_objMgr.count == 0);

    // objMgr doesn't own the objects, so just clean up self
    free(_objMgr.list);
    _objMgr.list = NULL;
    _objMgr.max = _objMgr.count = 0;
}

/// @brief Add an object to be tracked by the manager
/// @param obj 
void objMgrAdd(Object* obj)
{
    for (uint32_t i = 0; i < _objMgr.max; ++i)
    {
        if (_objMgr.list[i] == NULL)
        {
            _objMgr.list[i] = obj;
            ++_objMgr.count;
            return;
        }
    }

    // out of space to add object!
    assert(false);
}

/// @brief Remove an object from the manager's tracking
/// @param obj 
void objMgrRemove(Object* obj)
{
    for (uint32_t i = 0; i < _objMgr.max; ++i)
    {
        if (obj == _objMgr.list[i])
        {
            // no need to free memory, so just clear the reference
            _objMgr.list[i] = NULL;
            --_objMgr.count;
            return;
        }
    }

    // could not find object to remove!
    assert(false);
}

/// @brief Draws all registered objects
void objMgrDraw() 
{
    if (drawPause == true) return;

    for (uint32_t i = 0; i < _objMgr.max; ++i)
    {
        Object* obj = _objMgr.list[i];
        if (obj != NULL)
        {
            // TODO - consider draw order?
            objDraw(obj);
        }
    }
}

void objMgrPause()
{
    for (uint32_t i = 0; i < _objMgr.max; ++i)
    {
        Object* obj = _objMgr.list[i];
        if (obj != NULL)
        {
            obj->IsPaused = true;
        }
    }
}

void objMgrResume()
{
    for (uint32_t i = 0; i < _objMgr.max; ++i)
    {
        Object* obj = _objMgr.list[i];
        if (obj != NULL)
        {
            obj->IsPaused = false;
        }
    }
}

void objMgrPauseDrawing()
{
    drawPause = true;
}

void objMgrResumeDrawing()
{
    drawPause = false;
}

/// @brief Updates all registered objects
/// @param milliseconds 
void objMgrUpdate(uint32_t milliseconds)
{
    for (uint32_t i = 0; i < _objMgr.max; ++i)
    {
        Object* obj = _objMgr.list[i];
        if (obj != NULL)
        {
            objUpdate(obj, milliseconds);
        }
    }
}

void objCheckCollision()
{
    for (uint32_t i = 0; i < _objMgr.max - 1; ++i)
    {
        Object* sourceObj = _objMgr.list[i];
        if (sourceObj == NULL) continue;

        for (uint32_t j = i + 1; j < _objMgr.max; ++j)
        {
            Object* targetObj = _objMgr.list[j];
            if (targetObj == NULL) continue;

            // Checking for reverse cases too as a sanity check
            if (sourceObj->objtype == Character && targetObj->objtype == EnemyGrunt ||
                sourceObj->objtype == EnemyGrunt && targetObj->objtype == Character ||

                sourceObj->objtype == Character && targetObj->objtype == EnemyHulk ||
                sourceObj->objtype == EnemyHulk && targetObj->objtype == Character ||

                sourceObj->objtype == Character && targetObj->objtype == Obstacle ||
                sourceObj->objtype == Obstacle && targetObj->objtype == Character ||

                sourceObj->objtype == Character && targetObj->objtype == EnemyEnforcer ||
                sourceObj->objtype == EnemyEnforcer && targetObj->objtype == Character ||

                sourceObj->objtype == Character && targetObj->objtype == EnemyTank ||
                sourceObj->objtype == EnemyTank && targetObj->objtype == Character ||

                sourceObj->objtype == EnemyGrunt && targetObj->objtype == Obstacle ||
                sourceObj->objtype == Obstacle && targetObj->objtype == EnemyGrunt ||

                sourceObj->objtype == EnemyHulk && targetObj->objtype == Obstacle ||
                sourceObj->objtype == Obstacle && targetObj->objtype == EnemyHulk ||

                sourceObj->objtype == EnemyGrunt && targetObj->objtype == EnemyGrunt ||
                sourceObj->objtype == EnemyHulk && targetObj->objtype == EnemyHulk ||
                sourceObj->objtype == EnemySpheroid && targetObj->objtype == EnemySpheroid ||

                sourceObj->objtype == Projectile && targetObj->objtype == EnemyGrunt ||
                sourceObj->objtype == EnemyGrunt && targetObj->objtype == Projectile ||

                sourceObj->objtype == Projectile && targetObj->objtype == EnemyHulk ||
                sourceObj->objtype == EnemyHulk && targetObj->objtype == Projectile ||

                sourceObj->objtype == Projectile && targetObj->objtype == EnemySpheroid ||
                sourceObj->objtype == EnemySpheroid && targetObj->objtype == Projectile ||

                sourceObj->objtype == Projectile && targetObj->objtype == EnemyEnforcer ||
                sourceObj->objtype == EnemyEnforcer && targetObj->objtype == Projectile ||

                sourceObj->objtype == Projectile && targetObj->objtype == EnemyQuark ||
                sourceObj->objtype == EnemyQuark && targetObj->objtype == Projectile ||

                sourceObj->objtype == Projectile && targetObj->objtype == EnemyTank ||
                sourceObj->objtype == EnemyTank && targetObj->objtype == Projectile ||

                sourceObj->objtype == Projectile && targetObj->objtype == Obstacle ||
                sourceObj->objtype == Obstacle && targetObj->objtype == Projectile)
            {
                objCollTest(sourceObj, targetObj, sourceObj->objtype, targetObj->objtype);
            }
        }
    }
}

uint32_t GetCount()
{
    return _objMgr.count;
}
