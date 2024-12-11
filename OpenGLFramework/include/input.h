#pragma once
#include "baseTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    INPUT_BUTTON_RIGHT,
    INPUT_BUTTON_LEFT,

    INPUT_BUTTON_COUNT
} InputButton;

// "public" methods - use these to check keyboard & mouse status
bool inputKeyPressed(char vkCode);
Coord2D inputMousePosition();
bool inputMousePressed(InputButton button);

// "private" methods - should only be called by framework
void inputInit();
void inputShutdown();
void inputKeyUpdate(uint8_t vkCode, bool pressed);
void inputMouseUpdatePosition(Coord2D coords);
void inputMouseUpdateButton(InputButton button, bool pressed);

#ifdef __cplusplus
}
#endif
