#include "baseTypes.h"
#include "input.h"
#include "application.h"
#include "framework.h"

#include "levelmgr.h"
#include "objmgr.h"
#include <stdio.h>

static void _gameInit();
static void _gameShutdown();
static void _gameDraw();
static void _gameUpdate(uint32_t milliseconds);
static void _gameLevelUpdate(Level* newLevel);

static LevelDef _levelDefs[] = {
	{
		0,							// level number
	    {640,490},					// Main menu pos
	    {635,730},					// Start button pos
	    {640,500},					// Game over screen pos
		{{40, 40}, {1240, 920}},    // fieldBounds
		{{60, 60}, {1150, 850}},    // spawn outer bound
		{{100, 100}, {900, 700}},   // spawn inner bound
		{{150, 150}, {800, 700}},   // other types spawn outer bound
		{{250, 200}, {750, 650}},   // other types spawn inner bound
		0x00ffa500,					// fieldColor
	    {200, 20},					// Score UI position
	    {730, 260},					// Final Score UI position
	    {820, 410},					// High Scores UI position
	    {220, 20},					// Lives UI position
	    {460, 938},					// Wave text UI position
	    {400, 938},					// Wave number UI position
		100,						// Grunt score
		500,						// Spheroid score
		200,						// Enforcer score
		700,						// Quark score
		300,						// Tank score
		50,							// Simple obstacle score
		80,							// Complex obstacle score
		40,							// Static obstacle score
		100,						// Static 2084 obstacle score
		6,							// min number of grunts
		8,							// max number of grunts
		1,							// min grunt scale for waves
		2,							// max grunt scale for waves
		1,							// min number of hulks
		2,							// max number of hulks
		1,							// min hulk scale for waves
		2,							// max hulk scale for waves
		0,							// min number of spheroids
		1,							// max number of spheroids
		1,							// min spheroid scale for waves
		2,							// max spheroid scale for waves
		0,							// min number of quarks
		1,							// max number of quarks
		0,							// min quark scale for waves
		1,							// max quark scale for waves
		0,							// min number of simple obstacles
		2,							// max number of simple obstacles
		1,							// min simple obstacles scale for waves
		2,							// max simple obstacles scale for waves
		0,							// min number of complex obstacles
		1,							// max number of complex obstacles
		0,							// min complex obstacles scale for waves
		1,							// max complex obstacles scale for waves
		5,							// min number of static obstacles
		10,							// max number of static obstacles
		1,							// min static obstacles scale for waves
		2,							// max static obstacles scale for waves
		5,							// min number of static 2084 obstacles
		10,							// max number of complex obstacles
		1,							// min static 2084 obstacles scale for waves
		2,							// max static 2084 obstacles scale for waves
	}
};
static Level* _curLevel = NULL;

/// @brief Program Entry Point (WinMain)
/// @param hInstance 
/// @param hPrevInstance 
/// @param lpCmdLine 
/// @param nCmdShow 
/// @return 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	const char GAME_NAME[] = "Robotron 2084";

	Application* app = appNew(hInstance, GAME_NAME, _gameDraw, _gameUpdate);

	if (app != NULL)
	{
		GLWindow* window = fwInitWindow(app);
		if (window != NULL)
		{
			_gameInit();

			bool running = true;
			while (running)
			{
				running = fwUpdateWindow(window);
			}

			_gameShutdown();
			fwShutdownWindow(window);
		}

		appDelete(app);
	}
}

/// @brief Initialize code to run at application startup
static void _gameInit()
{
	const uint32_t MAX_OBJECTS = 800;
	objMgrInit(MAX_OBJECTS);
	levelMgrInit(&_levelDefs[0]);
	levelMgrSetUpdLevelCB(_gameLevelUpdate);

	_curLevel = levelMgrLoad(0);
}

/// @brief Cleanup the game and free up any allocated resources
static void _gameShutdown()
{
	levelMgrUnloadAll(_curLevel);
	levelMgrShutdown();
	objMgrShutdown();
}

/// @brief Draw everything to the screen for current frame
static void _gameDraw() 
{
	objMgrDraw();
	levelMgrDraw();
}

/// @brief Perform updates for all game objects, for the elapsed duration
/// @param milliseconds 
static void _gameUpdate(uint32_t milliseconds)
{
	// ESC exits the program
	if (inputKeyPressed(VK_ESCAPE))
	{
		// TODO 
		//TerminateApplication(window);
		exit(0);
	}

	// F1 toggles fullscreen
	if (inputKeyPressed(VK_F1))
	{
		// TODO 
		//ToggleFullscreen(window);
	}

	objMgrUpdate(milliseconds);
	objCheckCollision();
	levelMgrUpdate(milliseconds);
}

static void _gameLevelUpdate(Level* newLevel)
{
	_curLevel = newLevel;
}