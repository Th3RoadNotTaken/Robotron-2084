#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <gl/GLU.h>
#include "SOIL.h"

#include "Quark.h"
#include "random.h"
#include "stdbool.h"
#include "math.h"

static const char QuarkSprite[] = "asset/Quark.png";

static GLuint QuarkTexture = 0;

static void QuarkUpdate(Object* obj, uint32_t milliseconds);
static void QuarkDraw(Object* obj);
static void QuarkColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType);

static ObjVtable QuarkVtable = {
    QuarkDraw,
    QuarkUpdate
};
static ObjCollisionVtable QuarkCollVTable = {
    QuarkColl
};

void QuarkInitTextures()
{
    if (QuarkTexture == 0)
    {
        QuarkTexture = SOIL_load_OGL_texture(QuarkSprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
            SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB);
        assert(QuarkTexture != 0);
    }
}

void QuarkInitAnimations(Quark* Quark_Obj)
{
    assert(Quark_Obj != NULL);

    // Running left animations
    SpriteFrame* FlySpriteFrames = malloc(sizeof(SpriteFrame) * QUARK_FLY_ANIMS);
    assert(FlySpriteFrames != NULL);

    FlySpriteFrames[0].Position = (Coord2D){ QUARK_FLY_1_POS_X, QUARK_FLY_POS_Y };
    FlySpriteFrames[0].Size = (Coord2D){ QUARK_FLY_SIZE_X , QUARK_FLY_SIZE_Y };

    FlySpriteFrames[1].Position = (Coord2D){ QUARK_FLY_2_POS_X, QUARK_FLY_POS_Y };
    FlySpriteFrames[1].Size = (Coord2D){ QUARK_FLY_SIZE_X , QUARK_FLY_SIZE_Y };

    FlySpriteFrames[2].Position = (Coord2D){ QUARK_FLY_3_POS_X, QUARK_FLY_POS_Y };
    FlySpriteFrames[2].Size = (Coord2D){ QUARK_FLY_SIZE_X , QUARK_FLY_SIZE_Y };

    FlySpriteFrames[3].Position = (Coord2D){ QUARK_FLY_4_POS_X, QUARK_FLY_POS_Y };
    FlySpriteFrames[3].Size = (Coord2D){ QUARK_FLY_4_SIZE_X , QUARK_FLY_4_SIZE_Y };

    Quark_Obj->FlyAnim = InitAnimation(FlySpriteFrames, QUARK_FLY_ANIMS, 0, QUARK_FRAME_DUR);
}

Bounds2D CalculateQuarkBounds(Coord2D Position)
{
    float scaledWidth = QUARK_BOUND_X * QUARK_SPRITE_SCALE;
    float scaledHeight = QUARK_BOUND_Y * QUARK_SPRITE_SCALE;

    // calculate the bounding box
    float xPositionLeft = (Position.x - scaledWidth / 2);
    float xPositionRight = (Position.x + scaledWidth / 2);
    float yPositionTop = (Position.y - scaledHeight / 2);
    float yPositionBottom = (Position.y + scaledHeight / 2);

    return (Bounds2D) { { xPositionLeft, yPositionTop }, { xPositionRight, yPositionBottom } };
}

Quark* QuarkNew(Coord2D Position, Bounds2D FieldBounds, Robotron* Robotron_Obj, bool IsActive, ProjectileManager* Manager)
{
    Quark* Quark_Obj = malloc(sizeof(Quark));
    if (Quark_Obj != NULL)
    {
        float QuarkRandVel = randGetFloat(QUARK_MIN_VELOCITY, QUARK_MAX_VELOCITY);
        Coord2D Vel = { QuarkRandVel, QuarkRandVel };

        objInit(&Quark_Obj->Obj, &QuarkVtable, Position, Vel, CalculateQuarkBounds(Position), DefaultDir, DEFAULT_COLOR, DEFAULT_COLOR_CHANGE_DELAY, EnemyQuark, &QuarkCollVTable);

        QuarkInitAnimations(Quark_Obj);

        Quark_Obj->Bounds = FieldBounds;

        Quark_Obj->Obj.IsActive = IsActive;

        Quark_Obj->FlyElapsedTime = 0.0f;
        Quark_Obj->TankSpawnElapsedTime = 0.0f;

        // Set the Robotron character sprite sheet size
        Quark_Obj->SheetSize = (Coord2D){ 57.0f, 16.0f };

        Quark_Obj->Robotron_Obj = Robotron_Obj;

        Quark_Obj->Manager = Manager;
    }

    return Quark_Obj;
}

void QuarkDelete(Quark* Quark_Obj)
{
    assert(Quark_Obj != NULL);

    objDeinit(&Quark_Obj->Obj);

    free(Quark_Obj);
}

void QuarkDraw(Object* obj)
{
    assert(obj != NULL);
    Quark* Quark_Obj = (Quark*)obj;
    assert(Quark_Obj != NULL);

    if (obj->IsActive == false) return;

    SpriteFrame* QuarkFlyFrame = GetCurrentFrame(Quark_Obj->FlyAnim);

    assert(QuarkFlyFrame != NULL);

    // Enable blending to apply color tinting
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, QuarkTexture);
    // Disable anti-aliasing
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBegin(GL_TRIANGLE_STRIP);
    {
        // Scale the sprite
        GLfloat scaledWidth = QuarkFlyFrame->Size.x * QUARK_SPRITE_SCALE;
        GLfloat scaledHeight = QuarkFlyFrame->Size.y * QUARK_SPRITE_SCALE;

        // calculate the bounding box
        GLfloat xPositionLeft = (obj->position.x - scaledWidth / 2);
        GLfloat xPositionRight = (obj->position.x + scaledWidth / 2);
        GLfloat yPositionTop = (obj->position.y - scaledHeight / 2);
        GLfloat yPositionBottom = (obj->position.y + scaledHeight / 2);

        // Calculate the UV coordinates for the sprite based on its position in the sprite sheet
        GLfloat xTextureCoord = QuarkFlyFrame->Position.x / Quark_Obj->SheetSize.x;
        GLfloat yTextureCoord = 1.0f - (QuarkFlyFrame->Position.y / Quark_Obj->SheetSize.y);
        GLfloat uWidth = QuarkFlyFrame->Size.x / Quark_Obj->SheetSize.x;
        GLfloat vHeight = QuarkFlyFrame->Size.y / Quark_Obj->SheetSize.y;

        const float BG_DEPTH = -0.99f;

        GLubyte r, g, b;

        r = ((obj->color >> 16) & 0xFF);
        g = ((obj->color >> 8) & 0xFF);
        b = ((obj->color >> 0) & 0xFF);

        glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);

        // TL
        glTexCoord2f(xTextureCoord, yTextureCoord);
        glVertex3f(xPositionLeft, yPositionTop, BG_DEPTH);

        glColor4ub(r, g, b, 0xFF);

        // BL
        glTexCoord2f(xTextureCoord, yTextureCoord - vHeight);
        glVertex3f(xPositionLeft, yPositionBottom, BG_DEPTH);

        glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);

        // TR
        glTexCoord2f(xTextureCoord + uWidth, yTextureCoord);
        glVertex3f(xPositionRight, yPositionTop, BG_DEPTH);

        glColor4ub(r, g, b, 0xFF);

        // BR
        glTexCoord2f(xTextureCoord + uWidth, yTextureCoord - vHeight);
        glVertex3f(xPositionRight, yPositionBottom, BG_DEPTH);

    }
    glEnd();
}

void QuarkUpdate(Object* obj, uint32_t milliseconds)
{
    assert(obj != NULL);
    Quark* Quark_Obj = (Quark*)obj;
    assert(Quark_Obj != NULL && Quark_Obj->Manager != NULL);

    if (obj->IsActive == false || obj->IsPaused == true) return;

    PlayAnimation(Quark_Obj->FlyAnim, milliseconds);

    objSetRandomColor(obj, milliseconds);

    // Calculate bounds every frame
    obj->objBounds = CalculateQuarkBounds(obj->position);

    // Quark movement
    if (Quark_Obj->FlyElapsedTime >= Quark_Obj->FlyDuration)
    {
        // Generate a new waypoint after reaching the current one
        Quark_Obj->Obj.velocity.x = 0;
        Quark_Obj->Obj.velocity.y = 0;
        Quark_Obj->FlyElapsedTime = 0;
        Quark_Obj->FlyDuration = randGetFloat(QUARK_MIN_FLY_DURATION, QUARK_MAX_FLY_DURATION);

        //Quark_Obj->Obj.position = Quark_Obj->NextWaypoint;
        Quark_Obj->NextWaypoint = GenerateRandomWaypoint(Quark_Obj->Bounds);
    }
    else
    {
        // Move toward the waypoint with a curved motion
        Coord2D direction = {
            Quark_Obj->NextWaypoint.x - Quark_Obj->Obj.position.x,
            Quark_Obj->NextWaypoint.y - Quark_Obj->Obj.position.y
        };

        float distance = sqrtf(direction.x * direction.x + direction.y * direction.y);

        // Normalize direction
        if (distance > 4.0f) {
            direction.x /= distance;
            direction.y /= distance;
        }
        else
        {
            Quark_Obj->FlyElapsedTime = Quark_Obj->FlyDuration;
            return;
        }

        // Calculate perpendicular vector
        Coord2D perp = { -direction.y, direction.x };

        // Oscillation factor
        float oscillation = sinf(Quark_Obj->FlyElapsedTime * 0.005f) * 1.0f;

        // Combine direction and oscillation
        Coord2D finalDirection = {
            direction.x + perp.x * oscillation,
            direction.y + perp.y * oscillation
        };

        // Normalize final direction
        float finalMagnitude = sqrtf(finalDirection.x * finalDirection.x + finalDirection.y * finalDirection.y);
        finalDirection.x /= finalMagnitude;
        finalDirection.y /= finalMagnitude;

        // Update velocity
        float randVel = randGetFloat(QUARK_MIN_VELOCITY, QUARK_MAX_VELOCITY);

        Quark_Obj->Obj.velocity.x = finalDirection.x * randVel;
        Quark_Obj->Obj.velocity.y = finalDirection.y * randVel;

        // Update position
        Quark_Obj->Obj.position.x += Quark_Obj->Obj.velocity.x * milliseconds;
        Quark_Obj->Obj.position.y += Quark_Obj->Obj.velocity.y * milliseconds;

        // Increment elapsed time
        Quark_Obj->FlyElapsedTime += milliseconds;
    }

    // Quark tank spawning
    if (Quark_Obj->TankSpawnElapsedTime >= Quark_Obj->TankSpawnDelay)
    {
        Quark_Obj->TankSpawnElapsedTime = 0.0f;

        // Spawn tank
        ProjectileManagerSpawnTank(Quark_Obj->Manager, Quark_Obj->Obj.position);
    }
    else
    {
        Quark_Obj->TankSpawnElapsedTime += milliseconds;
    }
}

void QuarkColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType)
{
}
