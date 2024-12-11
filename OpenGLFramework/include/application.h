#pragma once
#include <Windows.h>
#include "baseTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct application_t Application;

typedef void (*AppDrawFunc)();
typedef void (*AppUpdateFunc)(uint32_t);

Application* appNew(HINSTANCE instance, const char* title, AppDrawFunc drawFunc, AppUpdateFunc updateFunc);
void appDelete(Application* app);
void appDraw(Application* app);
void appUpdate(Application* app, uint32_t milliseconds);

HINSTANCE appGetInstance(const Application* app);
const char* appGetTitle(const Application* app);

void appSetWidth(Application* app, uint32_t width);
void appSetHeight(Application* app, uint32_t height);
void appSetBitsPerPixel(Application* app, uint32_t bpp);
void appSetMaxSounds(Application* app, uint32_t maxSounds);

uint32_t appGetWidth(const Application* app);
uint32_t appGetHeight(const Application* app);
uint32_t appGetBitsPerPixel(const Application* app);
uint32_t appGetMaxSounds(const Application* app);

#ifdef __cplusplus
}
#endif