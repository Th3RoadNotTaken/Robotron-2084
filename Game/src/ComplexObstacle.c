#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <gl/GLU.h>
#include "SOIL.h"

#include "ComplexObstacle.h"
#include "random.h"
#include "stdbool.h"
#include "math.h"

static const char COBSSprite[] = "asset/ComplexObstacle.png";

static GLuint COBSTexture = 0;

static void COBSUpdate(Object* obj, uint32_t milliseconds);
static void COBSDraw(Object* obj);
static void COBSColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType);

static ObjVtable COBSVtable = {
	COBSDraw,
	COBSUpdate
};
static ObjCollisionVtable COBSCollVTable = {
	COBSColl
};

void COBSInitTextures()
{
	if (COBSTexture == 0)
	{
		COBSTexture = SOIL_load_OGL_texture(COBSSprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB);
		assert(COBSTexture != 0);
	}
}

void COBSInitAnimations(COBS* COBS_Obj)
{
    assert(COBS_Obj != NULL);

    // Running left animations
    SpriteFrame* MoveSpriteFrames = malloc(sizeof(SpriteFrame) * COBS_MOVE_ANIMS);
    assert(MoveSpriteFrames != NULL);

    MoveSpriteFrames[0].Position = (Coord2D){ COBS_MOVE_1_POS_X, COBS_MOVE_1_POS_Y };
    MoveSpriteFrames[0].Size = (Coord2D){ COBS_MOVE_1_SIZE_X , COBS_MOVE_1_SIZE_Y };

    MoveSpriteFrames[1].Position = (Coord2D){ COBS_MOVE_2_POS_X, COBS_MOVE_2_POS_Y };
    MoveSpriteFrames[1].Size = (Coord2D){ COBS_MOVE_2_SIZE_X , COBS_MOVE_2_SIZE_Y };

    MoveSpriteFrames[2].Position = (Coord2D){ COBS_MOVE_3_POS_X, COBS_MOVE_3_POS_Y };
    MoveSpriteFrames[2].Size = (Coord2D){ COBS_MOVE_3_SIZE_X , COBS_MOVE_3_SIZE_Y };

    MoveSpriteFrames[3].Position = (Coord2D){ COBS_MOVE_4_POS_X, COBS_MOVE_4_POS_Y };
    MoveSpriteFrames[3].Size = (Coord2D){ COBS_MOVE_4_SIZE_X , COBS_MOVE_4_SIZE_Y };

    MoveSpriteFrames[4].Position = (Coord2D){ COBS_MOVE_5_POS_X, COBS_MOVE_5_POS_Y };
    MoveSpriteFrames[4].Size = (Coord2D){ COBS_MOVE_5_SIZE_X , COBS_MOVE_5_SIZE_Y };

    MoveSpriteFrames[5].Position = (Coord2D){ COBS_MOVE_6_POS_X, COBS_MOVE_6_POS_Y };
    MoveSpriteFrames[5].Size = (Coord2D){ COBS_MOVE_6_SIZE_X , COBS_MOVE_6_SIZE_Y };

    MoveSpriteFrames[6].Position = (Coord2D){ COBS_MOVE_7_POS_X, COBS_MOVE_7_POS_Y };
    MoveSpriteFrames[6].Size = (Coord2D){ COBS_MOVE_7_SIZE_X , COBS_MOVE_7_SIZE_Y };

    MoveSpriteFrames[7].Position = (Coord2D){ COBS_MOVE_8_POS_X, COBS_MOVE_8_POS_Y };
    MoveSpriteFrames[7].Size = (Coord2D){ COBS_MOVE_8_SIZE_X , COBS_MOVE_8_SIZE_Y };

    COBS_Obj->MoveAnim = InitAnimation(MoveSpriteFrames, COBS_MOVE_ANIMS, 0, COBS_FRAME_DUR);
}

Bounds2D CalculateCOBSBounds(Coord2D Position)
{
    float scaledWidth = COBS_BOUND_X * COBS_SPRITE_SCALE;
    float scaledHeight = COBS_BOUND_Y * COBS_SPRITE_SCALE;

    // calculate the bounding box
    float xPositionLeft = (Position.x - scaledWidth / 2);
    float xPositionRight = (Position.x + scaledWidth / 2);
    float yPositionTop = (Position.y - scaledHeight / 2);
    float yPositionBottom = (Position.y + scaledHeight / 2);

    return (Bounds2D) { { xPositionLeft, yPositionTop }, { xPositionRight, yPositionBottom } };
}

COBS* COBSNew(Coord2D Position, Bounds2D FieldBounds, bool IsActive)
{
    COBS* COBS_Obj = malloc(sizeof(COBS));
    if (COBS_Obj != NULL)
    {
        float COBSRandVel = randGetFloat(COBS_MIN_VELOCITY, COBS_MAX_VELOCITY);
        Coord2D Vel = { COBSRandVel, COBSRandVel };

        objInit(&COBS_Obj->Obj, &COBSVtable, Position, Vel, CalculateCOBSBounds(Position), DefaultDir, DEFAULT_COLOR, DEFAULT_COLOR_CHANGE_DELAY, Obstacle, &COBSCollVTable);

        COBS_Obj->Obj.colorChangeDelay = 200;

        COBSInitAnimations(COBS_Obj);

        COBS_Obj->Bounds = FieldBounds;

        COBS_Obj->Obj.IsActive = IsActive;

        COBS_Obj->Obj.objSubType = ComplexObs;

        // Set the simple obstacle sprite sheet size
        COBS_Obj->SheetSize = (Coord2D){ 106.0f, 15.0f };
    }

    return COBS_Obj;
}

void COBSDelete(COBS* COBS_Obj)
{
    assert(COBS_Obj != NULL);

    objDeinit(&COBS_Obj->Obj);

    free(COBS_Obj);
}

void COBSDraw(Object* obj)
{
    assert(obj != NULL);
    COBS* COBS_Obj = (COBS*)obj;
    assert(COBS_Obj != NULL);

    if (obj->IsActive == false || obj->CanSpawn == false) return;

    // Get the current frame of the fly animation
    SpriteFrame* CurrentFrame = GetCurrentFrame(COBS_Obj->MoveAnim);

    assert(CurrentFrame != NULL);

    // Scale the sprite
    GLfloat scaledWidth = CurrentFrame->Size.x * COBS_SPRITE_SCALE;
    GLfloat scaledHeight = CurrentFrame->Size.y * COBS_SPRITE_SCALE;

    // calculate the bounding box
    GLfloat xPositionLeft = (obj->position.x - scaledWidth / 2);
    GLfloat xPositionRight = (obj->position.x + scaledWidth / 2);
    GLfloat yPositionTop = (obj->position.y - scaledHeight / 2);
    GLfloat yPositionBottom = (obj->position.y + scaledHeight / 2);

    // Calculate the UV coordinates for the sprite based on its position in the sprite sheet
    GLfloat xTextureCoord = CurrentFrame->Position.x / COBS_Obj->SheetSize.x;
    GLfloat yTextureCoord = 1.0f - (CurrentFrame->Position.y / COBS_Obj->SheetSize.y);
    GLfloat uWidth = CurrentFrame->Size.x / COBS_Obj->SheetSize.x;
    GLfloat vHeight = CurrentFrame->Size.y / COBS_Obj->SheetSize.y;

    const float BG_DEPTH = -0.99f;

    // Enable blending to apply color tinting
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, COBSTexture);
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

void COBSUpdate(Object* obj, uint32_t milliseconds)
{
    assert(obj != NULL);
    COBS* COBS_Obj = (COBS*)obj;
    assert(COBS_Obj != NULL);

    if (obj->IsActive == false || obj->IsPaused == true) return;

    if (obj->CanSpawn == false)
    {
        COBS_Obj->ElapsedSpawnTime += milliseconds;
        if (COBS_Obj->ElapsedSpawnTime >= COBS_Obj->SpawnDelay)
        {
            COBS_Obj->ElapsedSpawnTime = 0.0f;
            obj->CanSpawn = true;
        }
        else
        {
            return;
        }
    }

    objSetRandomColor(obj, milliseconds);

    PlayAnimation(COBS_Obj->MoveAnim, milliseconds);

    // Calculate bounds every frame
    obj->objBounds = CalculateCOBSBounds(obj->position);

    // Spheroid movement
    if (COBS_Obj->FlyElapsedTime >= COBS_Obj->FlyDuration)
    {
        // Generate a new waypoint after reaching the current one
        COBS_Obj->Obj.velocity.x = 0;
        COBS_Obj->Obj.velocity.y = 0;
        COBS_Obj->FlyElapsedTime = 0;
        COBS_Obj->FlyDuration = randGetFloat(COBS_MIN_FLY_DURATION, COBS_MAX_FLY_DURATION);

        //COBS_Obj->Obj.position = COBS_Obj->NextWaypoint;
        COBS_Obj->NextWaypoint = GenerateRandomWaypoint(COBS_Obj->Bounds);
    }
    else
    {
        // Move toward the waypoint with a curved motion
        Coord2D direction = {
            COBS_Obj->NextWaypoint.x - COBS_Obj->Obj.position.x,
            COBS_Obj->NextWaypoint.y - COBS_Obj->Obj.position.y
        };

        float distance = sqrtf(direction.x * direction.x + direction.y * direction.y);

        // Normalize direction
        if (distance > 2.0f) {
            direction.x /= distance;
            direction.y /= distance;
        }
        else
        {
            COBS_Obj->FlyElapsedTime = COBS_Obj->FlyDuration;
            return;
        }

        float randVel = randGetFloat(COBS_MIN_VELOCITY, COBS_MAX_VELOCITY);

        COBS_Obj->Obj.velocity.x = direction.x * randVel;
        COBS_Obj->Obj.velocity.y = direction.y * randVel;

        // Update position
        COBS_Obj->Obj.position.x += COBS_Obj->Obj.velocity.x * milliseconds;
        COBS_Obj->Obj.position.y += COBS_Obj->Obj.velocity.y * milliseconds;

        // Increment elapsed time
        COBS_Obj->FlyElapsedTime += milliseconds;
    }
}

void COBSColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType)
{
}