#pragma once
#include "baseTypes.h"
#include "Object.h"

#ifdef __cplusplus
extern "C" {
#endif

void objMgrInit(uint32_t maxObjects);
void objMgrShutdown();
void objMgrAdd(Object* obj);
void objMgrRemove(Object* obj);

void objMgrDraw();
void objMgrPause();
void objMgrResume();
void objMgrPauseDrawing();
void objMgrResumeDrawing();
void objMgrUpdate(uint32_t milliseconds);
void objCheckCollision();
uint32_t GetCount();

#ifdef __cplusplus
}
#endif