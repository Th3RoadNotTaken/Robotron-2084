#include <Windows.h>
#include <stdlib.h>
#include <assert.h>
#include <gl/GLU.h>
#include "SOIL.h"

#include "StaticObstacles.h"
#include "random.h"
#include "stdbool.h"
#include "math.h"

static const char OBSSprite[] = "asset/StaticObstacles.png";

static GLuint OBSTexture = 0;

static void OBSUpdate(Object* obj, uint32_t milliseconds);
static void OBSDraw(Object* obj);
static void OBS_2084_Update(Object* obj, uint32_t milliseconds);
static void OBS_2084_Draw(Object* obj);
static void OBSColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType);

static ObjVtable OBSVtable = {
	OBSDraw,
	OBSUpdate
};
static ObjVtable OBS_2084_Vtable = {
	OBS_2084_Draw,
	OBS_2084_Update
};
static ObjCollisionVtable OBSCollVTable = {
	OBSColl
};

void OBSInitTextures()
{
	if (OBSTexture == 0)
	{
		OBSTexture = SOIL_load_OGL_texture(OBSSprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB);
		assert(OBSTexture != 0);
	}
}

void OBSInitSprites(OBS* OBS_Obj)
{
	assert(OBS_Obj != NULL);

	OBS_Obj->OBS_SpriteFrames = malloc(sizeof(SpriteFrame) * OBS_NUM);
	assert(OBS_Obj->OBS_SpriteFrames != NULL);

	OBS_Obj->OBS_SpriteFrames[0].Position = (Coord2D){ OBS_1_POS_X, OBS_1_POS_Y };
	OBS_Obj->OBS_SpriteFrames[0].Size = (Coord2D){ OBS_1_SIZE_X , OBS_1_SIZE_Y };

	OBS_Obj->OBS_SpriteFrames[1].Position = (Coord2D){ OBS_2_POS_X, OBS_2_POS_Y };
	OBS_Obj->OBS_SpriteFrames[1].Size = (Coord2D){ OBS_2_SIZE_X , OBS_2_SIZE_Y };

	OBS_Obj->OBS_SpriteFrames[2].Position = (Coord2D){ OBS_3_POS_X, OBS_3_POS_Y };
	OBS_Obj->OBS_SpriteFrames[2].Size = (Coord2D){ OBS_3_SIZE_X , OBS_3_SIZE_Y };

	OBS_Obj->OBS_SpriteFrames[3].Position = (Coord2D){ OBS_4_POS_X, OBS_4_POS_Y };
	OBS_Obj->OBS_SpriteFrames[3].Size = (Coord2D){ OBS_4_SIZE_X , OBS_4_SIZE_Y };

	OBS_Obj->OBS_SpriteFrames[4].Position = (Coord2D){ OBS_5_POS_X, OBS_5_POS_Y };
	OBS_Obj->OBS_SpriteFrames[4].Size = (Coord2D){ OBS_5_SIZE_X , OBS_5_SIZE_Y };

	OBS_Obj->OBS_SpriteFrames[5].Position = (Coord2D){ OBS_6_POS_X, OBS_6_POS_Y };
	OBS_Obj->OBS_SpriteFrames[5].Size = (Coord2D){ OBS_6_SIZE_X , OBS_6_SIZE_Y };

	OBS_Obj->OBS_SpriteFrames[6].Position = (Coord2D){ OBS_7_POS_X, OBS_7_POS_Y };
	OBS_Obj->OBS_SpriteFrames[6].Size = (Coord2D){ OBS_7_SIZE_X , OBS_7_SIZE_Y };
}

void OBS_2084_InitSprites(OBS_2084* OBS_2084_Obj)
{
	assert(OBS_2084_Obj != NULL);

	OBS_2084_Obj->OBS_2084_SpriteFrame.Position = (Coord2D){ OBS_2084_POS_X , OBS_2084_POS_Y };
	OBS_2084_Obj->OBS_2084_SpriteFrame.Size = (Coord2D){ OBS_2084_SIZE_X , OBS_2084_SIZE_Y };
}

Bounds2D CalculateOBSBounds(Coord2D Position)
{
	float scaledWidth = OBS_BOUND_X * OBS_SPRITE_SCALE;
	float scaledHeight = OBS_BOUND_Y * OBS_SPRITE_SCALE;

	// calculate the bounding box
	float xPositionLeft = (Position.x - scaledWidth / 2);
	float xPositionRight = (Position.x + scaledWidth / 2);
	float yPositionTop = (Position.y - scaledHeight / 2);
	float yPositionBottom = (Position.y + scaledHeight / 2);

	return (Bounds2D) { { xPositionLeft, yPositionTop }, { xPositionRight, yPositionBottom } };
}

Bounds2D CalculateOBS_2084_Bounds(Coord2D Position)
{
	float scaledWidth = OBS_2084_SIZE_X * OBS_2084_SPRITE_SCALE;
	float scaledHeight = OBS_2084_SIZE_Y * OBS_2084_SPRITE_SCALE;

	// calculate the bounding box
	float xPositionLeft = (Position.x - scaledWidth / 2);
	float xPositionRight = (Position.x + scaledWidth / 2);
	float yPositionTop = (Position.y - scaledHeight / 2);
	float yPositionBottom = (Position.y + scaledHeight / 2);

	return (Bounds2D) { { xPositionLeft, yPositionTop }, { xPositionRight, yPositionBottom } };
}

OBS* OBSNew(Coord2D Position, Bounds2D FieldBounds, bool IsActive)
{
	OBS* OBS_Obj = malloc(sizeof(OBS));
	if (OBS_Obj != NULL)
	{
		objInit(&OBS_Obj->Obj, &OBSVtable, Position, (Coord2D) { 0, 0 }, CalculateOBSBounds(Position), DefaultDir, DEFAULT_COLOR, DEFAULT_COLOR_CHANGE_DELAY, Obstacle, & OBSCollVTable);

		OBS_Obj->Obj.colorChangeDelay = 200;

		OBSInitSprites(OBS_Obj);

		OBS_Obj->Bounds = FieldBounds;

		OBS_Obj->Obj.IsActive = IsActive;

		OBS_Obj->CurrentFrame = 0;

		OBS_Obj->Obj.objSubType = StaticObs;

		// Set the static obstacle sprite sheet size
		OBS_Obj->SheetSize = (Coord2D){ 93.0f, 10.0f };
	}

	return OBS_Obj;
}

OBS_2084* OBS_2084_New(Coord2D Position, Bounds2D FieldBounds, bool IsActive)
{
	OBS_2084* OBS_2084_Obj = malloc(sizeof(OBS_2084));
	if (OBS_2084_Obj != NULL)
	{
		objInit(&OBS_2084_Obj->Obj, &OBS_2084_Vtable, Position, (Coord2D) { 0, 0 }, CalculateOBS_2084_Bounds(Position), DefaultDir, DEFAULT_COLOR, DEFAULT_COLOR_CHANGE_DELAY, Obstacle, & OBSCollVTable);

		OBS_2084_Obj->Obj.colorChangeDelay = 200;

		OBS_2084_InitSprites(OBS_2084_Obj);

		OBS_2084_Obj->Bounds = FieldBounds;

		OBS_2084_Obj->Obj.IsActive = IsActive;

		OBS_2084_Obj->Obj.objSubType = StaticObs2084;

		// Set the static obstacle sprite sheet size
		OBS_2084_Obj->SheetSize = (Coord2D){ 93.0f, 10.0f };
	}

	return OBS_2084_Obj;
}

void OBSDelete(OBS* OBS_Obj)
{
	assert(OBS_Obj != NULL);

	objDeinit(&OBS_Obj->Obj);

	free(OBS_Obj);
}

void OBS_2084_Delete(OBS_2084* OBS_2084_Obj)
{
	assert(OBS_2084_Obj != NULL);

	objDeinit(&OBS_2084_Obj->Obj);

	free(OBS_2084_Obj);
}

void OBSDraw(Object* obj)
{
	assert(obj != NULL);
	OBS* OBS_Obj = (OBS*)obj;
	assert(OBS_Obj != NULL);

	if (obj->IsActive == false) return;

	// Draw the static obstacle spawned
	SpriteFrame Frame = OBS_Obj->OBS_SpriteFrames[OBS_Obj->CurrentFrame];
	// Scale the sprite
	GLfloat scaledWidth = Frame.Size.x * OBS_SPRITE_SCALE;
	GLfloat scaledHeight = Frame.Size.y * OBS_SPRITE_SCALE;

	// calculate the bounding box
	GLfloat xPositionLeft = (obj->position.x - scaledWidth / 2);
	GLfloat xPositionRight = (obj->position.x + scaledWidth / 2);
	GLfloat yPositionTop = (obj->position.y - scaledHeight / 2);
	GLfloat yPositionBottom = (obj->position.y + scaledHeight / 2);

	// Calculate the UV coordinates for the sprite based on its position in the sprite sheet
	GLfloat xTextureCoord = Frame.Position.x / OBS_Obj->SheetSize.x;
	GLfloat yTextureCoord = 1.0f - (Frame.Position.y / OBS_Obj->SheetSize.y);
	GLfloat uWidth = Frame.Size.x / OBS_Obj->SheetSize.x;
	GLfloat vHeight = Frame.Size.y / OBS_Obj->SheetSize.y;

	const float BG_DEPTH = -0.99f;

	// Enable blending to apply color tinting
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, OBSTexture);
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

		glColor4ub(r, g, b, 0xFF);

		// BL
		glTexCoord2f(xTextureCoord, yTextureCoord - vHeight);
		glVertex3f(xPositionLeft, yPositionBottom, BG_DEPTH);

		glColor4ub(r, g, b, 0xFF);

		// TR
		glTexCoord2f(xTextureCoord + uWidth, yTextureCoord);
		glVertex3f(xPositionRight, yPositionTop, BG_DEPTH);

		glColor4ub(r, g, b, 0xFF);

		// BR
		glTexCoord2f(xTextureCoord + uWidth, yTextureCoord - vHeight);
		glVertex3f(xPositionRight, yPositionBottom, BG_DEPTH);
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void OBSUpdate(Object* obj, uint32_t milliseconds)
{
	assert(obj != NULL);
	OBS* OBS_Obj = (OBS*)obj;
	assert(OBS_Obj != NULL);

	if (obj->IsActive == false || obj->IsPaused == true) return;

	objSetRandomColor(obj, milliseconds);

	// Calculate bounds every frame
	obj->objBounds = CalculateOBSBounds(obj->position);
}

void OBS_2084_Draw(Object* obj)
{
	assert(obj != NULL);
	OBS_2084* OBS_2084_Obj = (OBS_2084*)obj;
	assert(OBS_2084_Obj != NULL);

	if (obj->IsActive == false) return;

	// Draw the static obstacle spawned
	SpriteFrame Frame = OBS_2084_Obj->OBS_2084_SpriteFrame;
	// Scale the sprite
	GLfloat scaledWidth = Frame.Size.x * OBS_SPRITE_SCALE;
	GLfloat scaledHeight = Frame.Size.y * OBS_SPRITE_SCALE;

	// calculate the bounding box
	GLfloat xPositionLeft = (obj->position.x - scaledWidth / 2);
	GLfloat xPositionRight = (obj->position.x + scaledWidth / 2);
	GLfloat yPositionTop = (obj->position.y - scaledHeight / 2);
	GLfloat yPositionBottom = (obj->position.y + scaledHeight / 2);

	// Calculate the UV coordinates for the sprite based on its position in the sprite sheet
	GLfloat xTextureCoord = Frame.Position.x / OBS_2084_Obj->SheetSize.x;
	GLfloat yTextureCoord = 1.0f - (Frame.Position.y / OBS_2084_Obj->SheetSize.y);
	GLfloat uWidth = Frame.Size.x / OBS_2084_Obj->SheetSize.x;
	GLfloat vHeight = Frame.Size.y / OBS_2084_Obj->SheetSize.y;

	const float BG_DEPTH = -0.99f;

	// Enable blending to apply color tinting
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, OBSTexture);
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

		glColor4ub(r, g, b, 0xFF);

		// BL
		glTexCoord2f(xTextureCoord, yTextureCoord - vHeight);
		glVertex3f(xPositionLeft, yPositionBottom, BG_DEPTH);

		glColor4ub(r, g, b, 0xFF);

		// TR
		glTexCoord2f(xTextureCoord + uWidth, yTextureCoord);
		glVertex3f(xPositionRight, yPositionTop, BG_DEPTH);

		glColor4ub(r, g, b, 0xFF);

		// BR
		glTexCoord2f(xTextureCoord + uWidth, yTextureCoord - vHeight);
		glVertex3f(xPositionRight, yPositionBottom, BG_DEPTH);
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void OBS_2084_Update(Object* obj, uint32_t milliseconds)
{
	assert(obj != NULL);
	OBS_2084* OBS_2084_Obj = (OBS_2084*)obj;
	assert(OBS_2084_Obj != NULL);

	if (obj->IsActive == false || obj->IsPaused == true) return;

	objSetRandomColor(obj, milliseconds);

	// Calculate bounds every frame
	obj->objBounds = CalculateOBS_2084_Bounds(obj->position);
}

void OBSColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType)
{
}