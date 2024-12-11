#pragma once
#include "application.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gl_window_t GLWindow;

GLWindow* fwInitWindow(Application* app);
bool fwUpdateWindow(GLWindow* window);
void fwShutdownWindow(GLWindow* window);

void fwSendTerminate(GLWindow* window);
void fwSendFullscreen(GLWindow* window, bool fullscreen);
bool fwChangeResolution(GLWindow* window, uint32_t width, uint32_t height, uint32_t bitsPerPixel);

#ifdef __cplusplus
}
#endif