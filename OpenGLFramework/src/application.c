#include "application.h"

struct application_t {
    // windows instance
    HINSTANCE   instance;

    // application
    const char* title;
    AppDrawFunc drawFunc;
    AppUpdateFunc updateFunc;

    // window settings
    uint32_t    width;
    uint32_t    height;
    uint32_t    bpp;

    // audio
    uint32_t    maxSounds;
};

/// @brief Create an instance of an application with default settings
/// @param instance 
/// @param title 
/// @return 
Application* appNew(HINSTANCE instance, const char* title, AppDrawFunc drawFunc, AppUpdateFunc updateFunc)
{
    const uint32_t DEFAULT_WIDTH = 1280;
    const uint32_t DEFAULT_HEIGHT = 960;
    const uint32_t DEFAULT_BPP = 24;
    const uint32_t DEFAULT_MAXSOUNDS = 20;

    Application* app = malloc(sizeof(Application));
    if (app != NULL) {
        app->instance = instance;
        app->title = title;
        app->drawFunc = drawFunc;
        app->updateFunc = updateFunc;

        app->width = DEFAULT_WIDTH;
        app->height = DEFAULT_HEIGHT;
        app->bpp = DEFAULT_BPP;
        app->maxSounds = DEFAULT_MAXSOUNDS;
    }

    return app;
}

/// @brief Destroy an instance of an application and give back memory
/// @param application 
void appDelete(Application* application) 
{
    free(application);
}

/// @brief Does any required drawing for the application
/// @param application 
void appDraw(Application* app)
{
    if (app->drawFunc != NULL)
    {
        app->drawFunc();
    }
}

/// @brief Updates the application for the passage of the requested number of milliseconds
/// @param application 
/// @param milliseconds 
void appUpdate(Application* app, uint32_t milliseconds)
{
    if (app->updateFunc != NULL) 
    {
        app->updateFunc(milliseconds);
    }
}

/*
 * Additional setters for height, width and bits-per-pixel
 */
void appSetWidth(Application* app, uint32_t width) { app->width = width; }
void appSetHeight(Application* app, uint32_t height) { app->height = height; }
void appSetBitsPerPixel(Application* app, uint32_t bpp) { app->bpp = bpp; }
void appSetMaxSounds(Application* app, uint32_t maxSounds) { app->maxSounds = maxSounds; }

/*
 * Getters for various application fields
 */
HINSTANCE appGetInstance(const Application* app) { return app->instance; }
const char* appGetTitle(const Application* app) { return app->title; }
uint32_t appGetWidth(const Application* app) { return app->width; }
uint32_t appGetHeight(const Application* app) { return app->height; }
uint32_t appGetBitsPerPixel(const Application* app) { return app->bpp; }
uint32_t appGetMaxSounds(const Application* app) { return app->maxSounds; }
