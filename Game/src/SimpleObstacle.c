#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <gl/GLU.h>
#include "SOIL.h"

#include "SimpleObstacle.h"
#include "random.h"
#include "stdbool.h"
#include "math.h"

static const char SOBSSprite[] = "asset/SimpleObstacle.png";

static GLuint SOBSTexture = 0;

static void SOBSUpdate(Object* obj, uint32_t milliseconds);
static void SOBSDraw(Object* obj);
static void SOBSColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType);

static ObjVtable SOBSVtable = {
	SOBSDraw,
	SOBSUpdate
};
static ObjCollisionVtable SOBSCollVTable = {
	SOBSColl
};

void SOBSInitTextures()
{
	if (SOBSTexture == 0)
	{
		SOBSTexture = SOIL_load_OGL_texture(SOBSSprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB);
		assert(SOBSTexture != 0);
	}
}

void SOBSInitAnimations(SOBS* SOBS_Obj)
{
    assert(SOBS_Obj != NULL);

    // Running left animations
    SpriteFrame* MoveSpriteFrames = malloc(sizeof(SpriteFrame) * SOBS_MOVE_ANIMS);
    assert(MoveSpriteFrames != NULL);

    MoveSpriteFrames[0].Position = (Coord2D){ SOBS_MOVE_1_POS_X, SOBS_MOVE_1_POS_Y };
    MoveSpriteFrames[0].Size = (Coord2D){ SOBS_MOVE_1_SIZE_X , SOBS_MOVE_1_SIZE_Y };

    MoveSpriteFrames[1].Position = (Coord2D){ SOBS_MOVE_2_POS_X, SOBS_MOVE_2_POS_Y };
    MoveSpriteFrames[1].Size = (Coord2D){ SOBS_MOVE_2_SIZE_X , SOBS_MOVE_2_SIZE_Y };

    MoveSpriteFrames[2].Position = (Coord2D){ SOBS_MOVE_3_POS_X, SOBS_MOVE_3_POS_Y };
    MoveSpriteFrames[2].Size = (Coord2D){ SOBS_MOVE_3_SIZE_X , SOBS_MOVE_3_SIZE_Y };

    MoveSpriteFrames[3].Position = (Coord2D){ SOBS_MOVE_4_POS_X, SOBS_MOVE_4_POS_Y };
    MoveSpriteFrames[3].Size = (Coord2D){ SOBS_MOVE_4_SIZE_X , SOBS_MOVE_4_SIZE_Y };

    MoveSpriteFrames[4].Position = (Coord2D){ SOBS_MOVE_5_POS_X, SOBS_MOVE_5_POS_Y };
    MoveSpriteFrames[4].Size = (Coord2D){ SOBS_MOVE_5_SIZE_X , SOBS_MOVE_5_SIZE_Y };

    MoveSpriteFrames[5].Position = (Coord2D){ SOBS_MOVE_6_POS_X, SOBS_MOVE_6_POS_Y };
    MoveSpriteFrames[5].Size = (Coord2D){ SOBS_MOVE_6_SIZE_X , SOBS_MOVE_6_SIZE_Y };

    MoveSpriteFrames[6].Position = (Coord2D){ SOBS_MOVE_7_POS_X, SOBS_MOVE_7_POS_Y };
    MoveSpriteFrames[6].Size = (Coord2D){ SOBS_MOVE_7_SIZE_X , SOBS_MOVE_7_SIZE_Y };

    MoveSpriteFrames[7].Position = (Coord2D){ SOBS_MOVE_8_POS_X, SOBS_MOVE_8_POS_Y };
    MoveSpriteFrames[7].Size = (Coord2D){ SOBS_MOVE_8_SIZE_X , SOBS_MOVE_8_SIZE_Y };

    SOBS_Obj->MoveAnim = InitAnimation(MoveSpriteFrames, SOBS_MOVE_ANIMS, 0, SOBS_FRAME_DUR);
}

Bounds2D CalculateSOBSBounds(Coord2D Position)
{
    float scaledWidth = SOBS_BOUND_X * SOBS_SPRITE_SCALE;
    float scaledHeight = SOBS_BOUND_Y * SOBS_SPRITE_SCALE;

    // calculate the bounding box
    float xPositionLeft = (Position.x - scaledWidth / 2);
    float xPositionRight = (Position.x + scaledWidth / 2);
    float yPositionTop = (Position.y - scaledHeight / 2);
    float yPositionBottom = (Position.y + scaledHeight / 2);

    return (Bounds2D) { { xPositionLeft, yPositionTop }, { xPositionRight, yPositionBottom } };
}

SOBS* SOBSNew(Coord2D Position, Bounds2D FieldBounds, bool IsActive)
{
    SOBS* SOBS_Obj = malloc(sizeof(SOBS));
    if (SOBS_Obj != NULL)
    {
        float SOBSRandVel = randGetFloat(SOBS_MIN_VELOCITY, SOBS_MAX_VELOCITY);
        Coord2D Vel = { SOBSRandVel, SOBSRandVel };

        objInit(&SOBS_Obj->Obj, &SOBSVtable, Position, Vel, CalculateSOBSBounds(Position), DefaultDir, DEFAULT_COLOR, DEFAULT_COLOR_CHANGE_DELAY, Obstacle, &SOBSCollVTable);

        SOBS_Obj->Obj.colorChangeDelay = 200;

        SOBSInitAnimations(SOBS_Obj);

        SOBS_Obj->Bounds = FieldBounds;

        SOBS_Obj->Obj.IsActive = IsActive;

        SOBS_Obj->Obj.objSubType = SimpleObs;

        // Set the simple obstacle sprite sheet size
        SOBS_Obj->SheetSize = (Coord2D){ 92.0f, 15.0f };
    }

    return SOBS_Obj;
}

void SOBSDelete(SOBS* SOBS_Obj)
{
    assert(SOBS_Obj != NULL);

    objDeinit(&SOBS_Obj->Obj);

    free(SOBS_Obj);
}

void SOBSDraw(Object* obj)
{
    assert(obj != NULL);
    SOBS* SOBS_Obj = (SOBS*)obj;
    assert(SOBS_Obj != NULL);

    if (obj->IsActive == false || obj->CanSpawn == false) return;

    // Get the current frame of the fly animation
    SpriteFrame* CurrentFrame = GetCurrentFrame(SOBS_Obj->MoveAnim);

    assert(CurrentFrame != NULL);

    // Scale the sprite
    GLfloat scaledWidth = CurrentFrame->Size.x * SOBS_SPRITE_SCALE;
    GLfloat scaledHeight = CurrentFrame->Size.y * SOBS_SPRITE_SCALE;

    // calculate the bounding box
    GLfloat xPositionLeft = (obj->position.x - scaledWidth / 2);
    GLfloat xPositionRight = (obj->position.x + scaledWidth / 2);
    GLfloat yPositionTop = (obj->position.y - scaledHeight / 2);
    GLfloat yPositionBottom = (obj->position.y + scaledHeight / 2);

    // Calculate the UV coordinates for the sprite based on its position in the sprite sheet
    GLfloat xTextureCoord = CurrentFrame->Position.x / SOBS_Obj->SheetSize.x;
    GLfloat yTextureCoord = 1.0f - (CurrentFrame->Position.y / SOBS_Obj->SheetSize.y);
    GLfloat uWidth = CurrentFrame->Size.x / SOBS_Obj->SheetSize.x;
    GLfloat vHeight = CurrentFrame->Size.y / SOBS_Obj->SheetSize.y;

    const float BG_DEPTH = -0.99f;

    // Enable blending to apply color tinting
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, SOBSTexture);
    // Disable anti-aliasing
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBegin(GL_TRIANGLE_STRIP);
    {
        // draw the textured quad as a tristrip
        //glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);

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

void SOBSUpdate(Object* obj, uint32_t milliseconds)
{
    assert(obj != NULL);
    SOBS* SOBS_Obj = (SOBS*)obj;
    assert(SOBS_Obj != NULL);

    if (obj->IsActive == false || obj->IsPaused == true) return;

    if (obj->CanSpawn == false)
    {
        SOBS_Obj->ElapsedSpawnTime += milliseconds;
        if (SOBS_Obj->ElapsedSpawnTime >= SOBS_Obj->SpawnDelay)
        {
            SOBS_Obj->ElapsedSpawnTime = 0.0f;
            obj->CanSpawn = true;
        }
        else
        {
            return;
        }
    }

    objSetRandomColor(obj, milliseconds);

    PlayAnimation(SOBS_Obj->MoveAnim, milliseconds);

    // Calculate bounds every frame
    obj->objBounds = CalculateSOBSBounds(obj->position);

    // Check collision at boundary
    float rightSide = SOBS_Obj->Bounds.botRight.x;
    float leftSide = SOBS_Obj->Bounds.topLeft.x;
    float topSide = SOBS_Obj->Bounds.topLeft.y;
    float bottomSide = SOBS_Obj->Bounds.botRight.y;

    float scaledWidth = SOBS_BOUND_X * SOBS_SPRITE_SCALE;
    float scaledHeight = SOBS_BOUND_Y * SOBS_SPRITE_SCALE;

    float halfHeight = scaledHeight / 2.0f;
    float halfWidth = scaledWidth / 2.0f;

    if (SOBS_Obj->VelocityInitialized == false)
    {
        SOBS_Obj->NextWaypoint = GenerateRandomWaypoint(SOBS_Obj->Bounds);

        Coord2D direction = {
            SOBS_Obj->NextWaypoint.x - SOBS_Obj->Obj.position.x,
            SOBS_Obj->NextWaypoint.y - SOBS_Obj->Obj.position.y
        };

        float distance = sqrtf(direction.x * direction.x + direction.y * direction.y);
        // Normalize the distance
        if (distance > 0.0f) {
            direction.x /= distance;
            direction.y /= distance;
        }

        float randVel = randGetFloat(SOBS_MIN_VELOCITY, SOBS_MAX_VELOCITY);
        SOBS_Obj->Obj.velocity = (Coord2D){ randVel , randVel };

        SOBS_Obj->Obj.velocity.x = direction.x * randVel;
        SOBS_Obj->Obj.velocity.y = direction.y * randVel;

        SOBS_Obj->VelocityInitialized = true;
    }

    if (SOBS_Obj->Obj.position.x + SOBS_Obj->Obj.velocity.x - halfWidth <= leftSide || SOBS_Obj->Obj.position.x + SOBS_Obj->Obj.velocity.x + halfWidth >= rightSide)
        SOBS_Obj->Obj.velocity.x *= -1.0f;

    if (SOBS_Obj->Obj.position.y + SOBS_Obj->Obj.velocity.y + halfHeight >= bottomSide || SOBS_Obj->Obj.position.y + SOBS_Obj->Obj.velocity.y - halfHeight <= topSide)
        SOBS_Obj->Obj.velocity.y *= -1.0f;

    // Update position
    SOBS_Obj->Obj.position.x += SOBS_Obj->Obj.velocity.x * milliseconds;
    SOBS_Obj->Obj.position.y += SOBS_Obj->Obj.velocity.y * milliseconds;
}

void SOBSColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType)
{
}