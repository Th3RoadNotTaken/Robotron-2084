#include <Windows.h>
#include "baseTypes.h"
#include "input.h"

/// @brief Keyboard state
typedef struct {
	bool keyDown[256];
} Keyboard;

/// @brief Mouse state
typedef struct {
	Coord2D position;
	bool	buttons[INPUT_BUTTON_COUNT];
} Mouse;

static Keyboard s_Keyboard;
static Mouse s_Mouse;

/// @brief Retrieves the pressed state for a keyboard key
/// @param vkCode 
/// @return 
bool inputKeyPressed(char vkCode) 
{
	return s_Keyboard.keyDown[vkCode];
}

/// @brief Retrieves the current mouse position
/// @return 
Coord2D inputMousePosition() 
{
	return s_Mouse.position;
}

/// @brief Retrieves the pressed state for a mouse button
/// @param button 
/// @return 
bool inputMousePressed(InputButton button) 
{
	return s_Mouse.buttons[button];
}

/// @brief Input system initialization
void inputInit()
{
	ZeroMemory(&s_Keyboard, sizeof(Keyboard));
	ZeroMemory(&s_Mouse, sizeof(Mouse));
}

/// @brief Input system shutdown
void inputShutdown() 
{
	ZeroMemory(&s_Keyboard, sizeof(Keyboard));
	ZeroMemory(&s_Mouse, sizeof(Mouse));
}

/// @brief Updates the pressed state of a keyboard key
/// @param vkCode 
/// @param pressed 
void inputKeyUpdate(uint8_t vkCode, bool pressed) 
{
	s_Keyboard.keyDown[vkCode] = pressed;
}

/// @brief Updates the coordinates of the mouse
/// @param coords 
void inputMouseUpdatePosition(Coord2D coords) 
{
	s_Mouse.position = coords;
}

/// @brief Updates the pressed state of a mouse button
/// @param button 
/// @param pressed 
void inputMouseUpdateButton(InputButton button, bool pressed)
{
	s_Mouse.buttons[button] = pressed;
}


