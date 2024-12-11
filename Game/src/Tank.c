#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <gl/GLU.h>
#include "SOIL.h"

#include "Tank.h"
#include "random.h"
#include "stdbool.h"
#include "math.h"

static const char TankSprite[] = "asset/Tank.png";

static GLuint TankTexture = 0;

static void TankUpdate(Object* obj, uint32_t milliseconds);
static void TankDraw(Object* obj);
static void TankColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType);

static ObjVtable TankVtable = {
	TankDraw,
	TankUpdate
};
static ObjCollisionVtable TankCollVTable = {
	TankColl
};

void TankInitTextures()
{
	if (TankTexture == 0)
	{
		TankTexture = SOIL_load_OGL_texture(TankSprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB);
		assert(TankTexture != 0);
	}
}

void TankInitAnimations(Tank* Tank_Obj)
{
	assert(Tank_Obj != NULL);

	Tank_Obj->FlyFrame.Position = (Coord2D){ TANK_FLY_POS_X, TANK_FLY_POS_Y };
	Tank_Obj->FlyFrame.Size = (Coord2D){ TANK_FLY_SIZE_X, TANK_FLY_SIZE_Y };
}

Bounds2D CalculateTankBounds(Coord2D Position)
{
	float scaledWidth = TANK_BOUND_X * TANK_SPRITE_SCALE;
	float scaledHeight = TANK_BOUND_Y * TANK_SPRITE_SCALE;

	// calculate the bounding box
	float xPositionLeft = (Position.x - scaledWidth / 2);
	float xPositionRight = (Position.x + scaledWidth / 2);
	float yPositionTop = (Position.y - scaledHeight / 2);
	float yPositionBottom = (Position.y + scaledHeight / 2);

	return (Bounds2D) { { xPositionLeft, yPositionTop }, { xPositionRight, yPositionBottom } };
}

Tank* TankNew(Coord2D Position, Bounds2D FieldBounds, Robotron* Robotron_Obj, bool IsActive)
{
	Tank* Tank_Obj = malloc(sizeof(Tank));
	if (Tank_Obj != NULL)
	{
		float TankRandVel = randGetFloat(TANK_MIN_VELOCITY, TANK_MAX_VELOCITY);
		Coord2D Vel = { TankRandVel, TankRandVel };

		objInit(&Tank_Obj->Obj, &TankVtable, Position, Vel, CalculateTankBounds(Position), DefaultDir, DEFAULT_COLOR, DEFAULT_COLOR_CHANGE_DELAY, EnemyTank, &TankCollVTable);

		Tank_Obj->Obj.colorChangeDelay = 200;

		TankInitAnimations(Tank_Obj);

		Tank_Obj->Bounds = FieldBounds;

		Tank_Obj->Obj.IsActive = IsActive;

		// Set the Tank character sprite sheet size
		Tank_Obj->SheetSize = (Coord2D){ 7.0f, 7.0f };

		Tank_Obj->Robotron_Obj = Robotron_Obj;
	}

	return Tank_Obj;
}

void TankDelete(Tank* Tank_Obj)
{
	assert(Tank_Obj != NULL);

	objDeinit(&Tank_Obj->Obj);

	free(Tank_Obj);
}

void TankDraw(Object* obj)
{
    assert(obj != NULL);
    Tank* Tank_Obj = (Tank*)obj;
    assert(Tank_Obj != NULL);

    if (obj->IsActive == false) return;

    // Scale the sprite
    GLfloat scaledWidth = Tank_Obj->FlyFrame.Size.x * TANK_SPRITE_SCALE;
    GLfloat scaledHeight = Tank_Obj->FlyFrame.Size.y * TANK_SPRITE_SCALE;

    // calculate the bounding box
    GLfloat xPositionLeft = (obj->position.x - scaledWidth / 2);
    GLfloat xPositionRight = (obj->position.x + scaledWidth / 2);
    GLfloat yPositionTop = (obj->position.y - scaledHeight / 2);
    GLfloat yPositionBottom = (obj->position.y + scaledHeight / 2);

    // Calculate the UV coordinates for the sprite based on its position in the sprite sheet
    GLfloat xTextureCoord = Tank_Obj->FlyFrame.Position.x / Tank_Obj->SheetSize.x;
    GLfloat yTextureCoord = 1.0f - (Tank_Obj->FlyFrame.Position.y / Tank_Obj->SheetSize.y);
    GLfloat uWidth = Tank_Obj->FlyFrame.Size.x / Tank_Obj->SheetSize.x;
    GLfloat vHeight = Tank_Obj->FlyFrame.Size.y / Tank_Obj->SheetSize.y;

    const float BG_DEPTH = -0.99f;

    // Enable blending to apply color tinting
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, TankTexture);
    // Disable anti-aliasing
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBegin(GL_TRIANGLE_STRIP);
    {
        GLubyte r, g, b;

        r = ((obj->color >> 16) & 0xFF);
        g = ((obj->color >> 8) & 0xFF);
        b = ((obj->color >> 0) & 0xFF);

        glColor4ub(r, g, b, 0xFF);

        // TL
        glTexCoord2f(xTextureCoord, yTextureCoord);
        glVertex3f(xPositionLeft, yPositionTop, BG_DEPTH);

        // BL
        glTexCoord2f(xTextureCoord, yTextureCoord - vHeight);
        glVertex3f(xPositionLeft, yPositionBottom, BG_DEPTH);

        // TR
        glTexCoord2f(xTextureCoord + uWidth, yTextureCoord);
        glVertex3f(xPositionRight, yPositionTop, BG_DEPTH);

        // BR
        glTexCoord2f(xTextureCoord + uWidth, yTextureCoord - vHeight);
        glVertex3f(xPositionRight, yPositionBottom, BG_DEPTH);
    }
    glEnd();
}

void TankUpdate(Object* obj, uint32_t milliseconds)
{
    assert(obj != NULL);
    Tank* Tank_Obj = (Tank*)obj;
    assert(Tank_Obj != NULL);

    if (obj->IsActive == false || obj->IsPaused == true) return;

    objSetRandomColor(obj, milliseconds);

    // Calculate bounds every frame
    obj->objBounds = CalculateTankBounds(obj->position);

    // Move towards the initial player position
    if (Tank_Obj->VelocityInitialized == false)
    {
        Coord2D direction = {
            Tank_Obj->TargetPoint.x - Tank_Obj->Obj.position.x,
            Tank_Obj->TargetPoint.y - Tank_Obj->Obj.position.y
        };

        float distance = sqrtf(direction.x * direction.x + direction.y * direction.y);
        // Normalize the distance
        if (distance > 0.0f) {
            direction.x /= distance;
            direction.y /= distance;
        }

        // Update velocity
        float randVel = randGetFloat(TANK_MIN_VELOCITY, TANK_MAX_VELOCITY);

        Tank_Obj->Obj.velocity.x = direction.x * randVel;
        Tank_Obj->Obj.velocity.y = direction.y * randVel;

        Tank_Obj->VelocityInitialized = true;
    }

    // Check collision with boundary. If colliding, reverse velocity
    float rightSide = Tank_Obj->Bounds.botRight.x;
    float leftSide = Tank_Obj->Bounds.topLeft.x;
    float topSide = Tank_Obj->Bounds.topLeft.y;
    float bottomSide = Tank_Obj->Bounds.botRight.y;

    float scaledWidth = TANK_BOUND_X * TANK_SPRITE_SCALE;
    float scaledHeight = TANK_BOUND_Y * TANK_SPRITE_SCALE;

    float halfHeight = scaledHeight / 2.0f;
    float halfWidth = scaledWidth / 2.0f;

    if (Tank_Obj->Obj.position.x + (Tank_Obj->Obj.velocity.x * milliseconds) - halfWidth <= leftSide || Tank_Obj->Obj.position.x + (Tank_Obj->Obj.velocity.x * milliseconds) + halfWidth >= rightSide)
    {
        Tank_Obj->Obj.velocity.x *= -1.0f;
    }

    if (Tank_Obj->Obj.position.y + (Tank_Obj->Obj.velocity.y * milliseconds) + halfHeight >= bottomSide || Tank_Obj->Obj.position.y + (Tank_Obj->Obj.velocity.y * milliseconds) - halfHeight <= topSide)
    {
        Tank_Obj->Obj.velocity.y *= -1.0f;
    }
    
    // Update position
    Tank_Obj->Obj.position.x += Tank_Obj->Obj.velocity.x * milliseconds;
    Tank_Obj->Obj.position.y += Tank_Obj->Obj.velocity.y * milliseconds;
}

void TankColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType)
{
}