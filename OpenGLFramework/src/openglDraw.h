#pragma once
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

/// @brief Initialize the Open GL rendering system
/// @param backRed 
/// @param backGreen 
/// @param backBlue 
inline void glDrawInit(float backRed, float backGreen, float backBlue)
{
	glClearColor(backRed, backGreen, backBlue, 0.0f);			// Background Color
	glClearDepth(1.0f);											// Depth Buffer Setup
	glDepthFunc(GL_LESS);										// Type Of Depth Testing
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);			// Enable Alpha Blending
	glEnable(GL_DEPTH_TEST);									// Enable Depth Testing
	glEnable(GL_BLEND);											// Enable Blending
	glEnable(GL_TEXTURE_2D);									// Enable Texture Mapping
	glEnable(GL_CULL_FACE);										// Remove Back Face
}

/// @brief Begin drawing GL primitives for the current frame
inline void glDrawStart()
{
	// Clear the window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Set the modelview matrix to be the identity matrix
	glLoadIdentity();
}

/// @brief End GL drawing primitives for the current frame
inline void glDrawEnd()
{
	glFlush();
}

/// @brief Update GL for the current screen size
/// @param width 
/// @param height 
inline void glDrawResize(int32_t width, int32_t height)
{
	// reset the viewport and projection matrix
	glViewport(0, 0, (GLsizei)(width), (GLsizei)(height));
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Define the dimensions of the Orthographic Viewing Volume
	glOrtho(0.0, width, height, 0.0, -1.0, 1.0);

	// Reset The Modelview Matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}