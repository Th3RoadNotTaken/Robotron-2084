#include <Windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#include "framework.h"
#include "openglDraw.h"
#include "input.h"
#include "sound.h"

// Application Define Message For Toggling
#define WM_TOGGLEFULLSCREEN (WM_USER+1)

// CDS_FULLSCREEN Is Not Defined By Some Compilers. By Defining It This Way,
// We Can Avoid Errors
#ifndef		CDS_FULLSCREEN
#define		CDS_FULLSCREEN 4
#endif

static const char CLASS_NAME[] = "OpenGL Application";

typedef struct gl_window_t {						// Contains Information Vital To A Window
	Application*		app;

	// associated windows handles
	HWND				hWnd;						// Window Handle
	HDC					hDC;						// Device Context
	HGLRC				hRC;						// Rendering Context

	// state information
	bool				isVisible;					// Window Visible?
	ULONGLONG			lastTickCount;				// Tick Counter
} GLWindow;

// private helper methods
static bool _registerWindowClass(Application* app);
static GLWindow* _createWindow(Application* app);
static void _destroyWindow(GLWindow* window);
static HWND _initializeWindowEx(GLWindow* window, Application* app);
static bool _setPixelFormat(HDC deviceContext, uint32_t bitsPerPixel);
static LRESULT CALLBACK _messageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/// @brief Initialize windows for running this application
/// @param app 
/// @return 
GLWindow* fwInitWindow(Application* app)
{
	AllocConsole();

	// setup standard output channel for windows
	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((intptr_t)handle_out, _O_TEXT);
	FILE* hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 1);
	*stdout = *hf_out;

	// setup standard input channel for windows
	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	hCrt = _open_osfhandle((intptr_t)handle_in, _O_TEXT);
	FILE* hf_in = _fdopen(hCrt, "r");
	setvbuf(hf_in, NULL, _IONBF, 128);
	*stdin = *hf_in;

	// initialize core systems
	soundInit(appGetMaxSounds(app));
	inputInit();

	// Register A Class For Our Window To Use
	if (!_registerWindowClass(app))
	{
		MessageBox(HWND_DESKTOP, "Error Registering Window Class!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return NULL;
	}

	GLWindow* window = _createWindow(app);
	if (window == NULL)
	{
		MessageBox(HWND_DESKTOP, "Error Creating GL Window!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return NULL;
	}

	const float BG_RED = 0.0f;
	const float BG_GREEN = 0.0f;
	const float BG_BLUE = 0.0f;

	glDrawInit(BG_RED, BG_GREEN, BG_BLUE);

	return window;
}

bool fwUpdateWindow(GLWindow* window)
{
	MSG msg;

	if (PeekMessage(&msg, window->hWnd, 0, 0, PM_REMOVE) != 0) 
	{
		if (msg.message == WM_QUIT)
		{
			return false;
		}
		else
		{
			DispatchMessage(&msg);
		}
	}
	else
	{
		if (window->isVisible) 
		{
			ULONGLONG tickCount = GetTickCount64();
			uint32_t ticks = (uint32_t)(tickCount - window->lastTickCount);
			window->lastTickCount = tickCount;

			appUpdate(window->app, ticks);

			glDrawStart();
			appDraw(window->app);
			glDrawEnd();

			SwapBuffers(window->hDC);
		}
		else
		{
			WaitMessage();
		}
	}

	return true;
}

/// @brief Safely shutdown resources associated with this window and free up memory
/// @param window 
void fwShutdownWindow(GLWindow* window)
{
	// store the instance, so we can still safely use it after destroying the window
	HINSTANCE inst = appGetInstance(window->app);

	inputShutdown();
	soundShutdown();

	_destroyWindow(window);

	// UnRegister Window Class
	UnregisterClass(CLASS_NAME, inst);
}

/// @brief Sends a message to terminate the application
/// @param window 
void fwSendTerminate(GLWindow* window) 
{
	// Send A WM_QUIT Message
	PostMessage(window->hWnd, WM_QUIT, 0, 0);
}

/// @brief Sends a message to update the fullscreen state
/// @param window 
/// @param fullscreen 
void fwSendFullscreen(GLWindow* window, bool fullscreen)
{
	// Send A WM_TOGGLEFULLSCREEN Message
	PostMessage(window->hWnd, WM_TOGGLEFULLSCREEN, (WPARAM)fullscreen, 0);
}

/// @brief Performs a resolution change to the selected values
/// @param window 
/// @param width 
/// @param height 
/// @param bitsPerPixel 
/// @return 
bool fwChangeResolution(GLWindow* window, uint32_t width, uint32_t height, uint32_t bitsPerPixel)
{
	DEVMODE dmScreenSettings;								// Device Mode
	ZeroMemory(&dmScreenSettings, sizeof(DEVMODE));			// Make Sure Memory Is Cleared
	dmScreenSettings.dmSize = sizeof(DEVMODE);				// Size Of The Devmode Structure
	dmScreenSettings.dmPelsWidth = width;					// Select Screen Width
	dmScreenSettings.dmPelsHeight = height;					// Select Screen Height
	dmScreenSettings.dmBitsPerPel = bitsPerPixel;			// Select Bits Per Pixel
	dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	{
		// Display Change Failed, Return False
		return false;
	}
	return true;
}

/// @brief Registers a windows class (primarily for message handling)
/// @param app 
/// @return 
static bool _registerWindowClass(Application * app)
{
	// Register A Window Class
	WNDCLASSEX windowClass;
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = (WNDPROC)(_messageHandler);
	windowClass.hInstance = appGetInstance(app);
	windowClass.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = CLASS_NAME;

	if (RegisterClassEx(&windowClass) == 0)
	{
		// NOTE: Failure, Should Never Happen
		MessageBox(HWND_DESKTOP, "RegisterClassEx Failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	return true;
}

/// @brief Create and register a GL window with windows
/// @param app 
/// @return 
static GLWindow* _createWindow(Application* app) 
{
	GLWindow* window = malloc(sizeof(GLWindow));
	if (window != NULL) 
	{
		ZeroMemory(window, sizeof(GLWindow));

		window->app = app;

		// create the GL window
		window->hWnd = _initializeWindowEx(window, app);
		if (window->hWnd == 0)
		{
			_destroyWindow(window);
			return NULL;
		}

		// establish the device context
		window->hDC = GetDC(window->hWnd);									// Grab A Device Context For This Window
		if (window->hDC == 0)												// Did We Get A Device Context?
		{
			_destroyWindow(window);
			return NULL;
		}

		// setup the default pixel format
		if (!_setPixelFormat(window->hDC, appGetBitsPerPixel(app)))
		{
			_destroyWindow(window);
			return NULL;
		}

		// establish the rendering context
		window->hRC = wglCreateContext(window->hDC);						// Try To Get A Rendering Context
		if (window->hRC == 0)												// Did We Get A Rendering Context?
		{
			_destroyWindow(window);
			return NULL;
		}

		// Make The Rendering Context Our Current Rendering Context
		if (!wglMakeCurrent(window->hDC, window->hRC))
		{
			_destroyWindow(window);
			return NULL;
		}

		// Make The Window Visible
		ShowWindow(window->hWnd, SW_NORMAL);
		window->isVisible = true;

		// Reshape Our GL Window
		glDrawResize(appGetWidth(app), appGetHeight(app));

		// Get Tick Count
		window->lastTickCount = GetTickCount64();
	}

	return window;
}

/// @brief Safely clean up and destroy a GL window
/// @param window 
static void _destroyWindow(GLWindow* window) 
{
	ShowCursor(TRUE);

	// ensure clean processing of the "failed allocation" case
	if (window == NULL) {
		return;
	}

	// clean up the rendering context
	if (window->hRC != 0) 
	{
		wglDeleteContext(window->hRC);
	}

	// clean up the device context
	if (window->hDC != 0) 
	{
		ReleaseDC(window->hWnd, window->hDC);
	}

	// clean up the windows window
	if (window->hWnd != 0)
	{
		DestroyWindow(window->hWnd);
	}

	// finally, free up the memory!
	free(window);
}

/// @brief Bind this application to a physical window in windows
/// @param window 
/// @param app 
/// @return 
static HWND _initializeWindowEx(GLWindow* window, Application* app) 
{
	ShowCursor(FALSE);

	// Define Our Window Style & default coordinates
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;
	DWORD windowExtendedStyle = WS_EX_APPWINDOW;
	RECT windowRect = { 0, 0, appGetWidth(app), appGetHeight(app) };

	// Adjust Window, Account For Window Borders
	AdjustWindowRectEx(&windowRect, windowStyle, 0, windowExtendedStyle);

	// Create The OpenGL Window
	return CreateWindowEx(
		windowExtendedStyle,				// Extended Style
		CLASS_NAME,							// Class Name
		appGetTitle(app),					// Window Title
		windowStyle,						// Window Style
		0, 0,								// Window X,Y Position
		windowRect.right - windowRect.left,	// Window Width
		windowRect.bottom - windowRect.top,	// Window Height
		HWND_DESKTOP,						// Desktop Is Window's Parent
		0,									// No Menu
		appGetInstance(app),				// Pass The Window Instance
		window								// Context pointer (self)
	);
}

/// @brief Establish and set up the pixel format that will be used
/// @param deviceContext 
/// @param bitsPerPixel 
/// @return 
static bool _setPixelFormat(HDC deviceContext, uint32_t bitsPerPixel) 
{
	PIXELFORMATDESCRIPTOR pfd =											// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),									// Size Of This Pixel Format Descriptor
		1,																// Version Number
		PFD_DRAW_TO_WINDOW |											// Format Must Support Window
		PFD_SUPPORT_OPENGL |											// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,												// Must Support Double Buffering
		PFD_TYPE_RGBA,													// Request An RGBA Format
		(BYTE)bitsPerPixel,												// Select Our Color Depth
		0, 0, 0, 0, 0, 0,												// Color Bits Ignored
		0,																// No Alpha Buffer
		0,																// Shift Bit Ignored
		0,																// No Accumulation Buffer
		0, 0, 0, 0,														// Accumulation Bits Ignored
		16,																// 16Bit Z-Buffer (Depth Buffer)  
		0,																// No Stencil Buffer
		0,																// No Auxiliary Buffer
		PFD_MAIN_PLANE,													// Main Drawing Layer
		0,																// Reserved
		0, 0, 0															// Layer Masks Ignored
	};

	// Find A Compatible Pixel Format
	GLuint pixelFormat = ChoosePixelFormat(deviceContext, &pfd);

	return SetPixelFormat(deviceContext, pixelFormat, &pfd);
}

/// @brief Processes incoming windows messages
/// @param hWnd 
/// @param uMsg unused
/// @param wParam unused
/// @param lParam unused
/// @return 
static LRESULT CALLBACK _messageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Get The Window Context
	GLWindow* window = (GLWindow*)(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	switch (uMsg)														// Evaluate Window Message
	{
		case WM_SYSCOMMAND:												// Intercept System Commands
		{
			switch (wParam)												// Check System Calls
			{
				case SC_SCREENSAVE:										// Screensaver Trying To Start?
				case SC_MONITORPOWER:									// Monitor Trying To Enter Powersave?
					return 0;												// Prevent From Happening
			}
			break;
		}
		return 0;

		case WM_CREATE:													// Window Creation
		{
			CREATESTRUCT* creation = (CREATESTRUCT*)(lParam);			// Store Window Structure Pointer
			window = (GLWindow*)(creation->lpCreateParams);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)window);
		}
		return 0;

		case WM_CLOSE:													// Closing The Window
			fwSendTerminate(window);									// Terminate The Application
			return 0;

		case WM_SIZE:													// Size Action Has Taken Place
			switch (wParam)												// Evaluate Size Action
			{
				case SIZE_MINIMIZED:									// Was Window Minimized?
					window->isVisible = FALSE;							// Set isVisible To False
					return 0;

				case SIZE_MAXIMIZED:									// Was Window Maximized?
					window->isVisible = TRUE;							// Set isVisible To True
					glDrawResize(LOWORD(lParam), HIWORD(lParam));		// Reshape Window - LoWord=Width, HiWord=Height
					return 0;

				case SIZE_RESTORED:										// Was Window Restored?
					window->isVisible = TRUE;							// Set isVisible To True
					glDrawResize(LOWORD(lParam), HIWORD(lParam));		// Reshape Window - LoWord=Width, HiWord=Height
					return 0;
			}
			break;

		//case WM_TOGGLEFULLSCREEN:										// Toggle FullScreen Mode On/Off
		//	g_createFullScreen = (g_createFullScreen == TRUE) ? FALSE : TRUE;
		//	PostMessage(hWnd, WM_QUIT, 0, 0);
		//	break;

		/***
		 * Mouse inputs
		 ***/
		case WM_RBUTTONDOWN:	inputMouseUpdateButton(INPUT_BUTTON_RIGHT, true); return 0;
		case WM_RBUTTONUP:		inputMouseUpdateButton(INPUT_BUTTON_RIGHT, false); return 0;
		case WM_LBUTTONDOWN:	inputMouseUpdateButton(INPUT_BUTTON_LEFT, true); return 0;
		case WM_LBUTTONUP:		inputMouseUpdateButton(INPUT_BUTTON_LEFT, true); return 0;

		case WM_MOUSEMOVE:
		{
			Coord2D coord;
			coord.x = (float)LOWORD(lParam);
			coord.y = (float)HIWORD(lParam);

			inputMouseUpdatePosition(coord);
			return 0;
		}

		/***
		 * Keyboard inputs
		 ***/
		case WM_KEYDOWN:
			// Is Key (wParam) In A Valid Range?
			if ((wParam >= 0) && (wParam <= 255))
			{
				inputKeyUpdate((uint8_t)wParam, true);
				return 0;
			}
			break;

		case WM_KEYUP:
			// Is Key (wParam) In A Valid Range?
			if ((wParam >= 0) && (wParam <= 255))
			{
				inputKeyUpdate((uint8_t)wParam, false);
				return 0;
			}
			break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);					// Pass Unhandled Messages To DefWindowProc
}

