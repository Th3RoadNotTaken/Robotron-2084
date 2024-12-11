#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <gl/GLU.h>
#include "SOIL.h"

#include "Spheroid.h"
#include "random.h"
#include "stdbool.h"
#include "math.h"

static const char SpheroidSprite[] = "asset/Spheroid_White.png";

static GLuint SpheroidTexture = 0;

static void SpheroidUpdate(Object* obj, uint32_t milliseconds);
static void SpheroidDraw(Object* obj);
static void SpheroidColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType);

static ObjVtable SpheroidVtable = {
    SpheroidDraw,
    SpheroidUpdate
};
static ObjCollisionVtable SpheroidCollVTable = {
    SpheroidColl
};

void SpheroidInitTextures()
{
	if (SpheroidTexture == 0)
	{
        SpheroidTexture = SOIL_load_OGL_texture(SpheroidSprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
            SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB);
        assert(SpheroidTexture != 0);
	}
}

void SpheroidInitAnimations(Spheroid* Spheroid_Obj)
{
    assert(Spheroid_Obj != NULL);

    // Running left animations
    SpriteFrame* SpawnSpriteFrames = malloc(sizeof(SpriteFrame) * SPHEROID_SPAWN_ANIMS);
    assert(SpawnSpriteFrames != NULL);

    SpawnSpriteFrames[0].Position = (Coord2D){ SPHEROID_SPAWN_1_POS_X, SPHEROID_SPAWN_1_POS_Y };
    SpawnSpriteFrames[0].Size = (Coord2D){ SPHEROID_SPAWN_1_SIZE_X , SPHEROID_SPAWN_1_SIZE_Y };

    SpawnSpriteFrames[1].Position = (Coord2D){ SPHEROID_SPAWN_2_POS_X, SPHEROID_SPAWN_2_POS_Y };
    SpawnSpriteFrames[1].Size = (Coord2D){ SPHEROID_SPAWN_2_SIZE_X , SPHEROID_SPAWN_2_SIZE_Y };

    SpawnSpriteFrames[2].Position = (Coord2D){ SPHEROID_SPAWN_3_POS_X, SPHEROID_SPAWN_3_POS_Y };
    SpawnSpriteFrames[2].Size = (Coord2D){ SPHEROID_SPAWN_3_SIZE_X , SPHEROID_SPAWN_3_SIZE_Y };

    SpawnSpriteFrames[3].Position = (Coord2D){ SPHEROID_SPAWN_4_POS_X, SPHEROID_SPAWN_4_POS_Y };
    SpawnSpriteFrames[3].Size = (Coord2D){ SPHEROID_SPAWN_4_SIZE_X , SPHEROID_SPAWN_4_SIZE_Y };

    SpawnSpriteFrames[4].Position = (Coord2D){ SPHEROID_SPAWN_5_POS_X, SPHEROID_SPAWN_5_POS_Y };
    SpawnSpriteFrames[4].Size = (Coord2D){ SPHEROID_SPAWN_5_SIZE_X , SPHEROID_SPAWN_5_SIZE_Y };

    SpawnSpriteFrames[5].Position = (Coord2D){ SPHEROID_SPAWN_6_POS_X, SPHEROID_SPAWN_6_POS_Y };
    SpawnSpriteFrames[5].Size = (Coord2D){ SPHEROID_SPAWN_6_SIZE_X , SPHEROID_SPAWN_6_SIZE_Y };

    Spheroid_Obj->SpawnAnim = InitAnimation(SpawnSpriteFrames, SPHEROID_SPAWN_ANIMS, 0, SPHEROID_FRAME_DUR);
}

void SpheroidPlaySpawnAnimation(Spheroid* Spheroid_Obj, uint32_t milliseconds)
{
    assert(Spheroid_Obj != NULL);

    PlayAnimation(Spheroid_Obj->SpawnAnim, milliseconds);
}

Bounds2D CalculateSpheroidBounds(Coord2D Position)
{
    float scaledWidth = SPHEROID_BOUND_X * SPHEROID_SPRITE_SCALE;
    float scaledHeight = SPHEROID_BOUND_Y * SPHEROID_SPRITE_SCALE;

    // calculate the bounding box
    float xPositionLeft = (Position.x - scaledWidth / 2);
    float xPositionRight = (Position.x + scaledWidth / 2);
    float yPositionTop = (Position.y - scaledHeight / 2);
    float yPositionBottom = (Position.y + scaledHeight / 2);

    return (Bounds2D) { { xPositionLeft, yPositionTop }, { xPositionRight, yPositionBottom } };
}

Spheroid* SpheroidNew(Coord2D Position, Bounds2D FieldBounds, Robotron* Robotron_Obj, bool IsActive, ProjectileManager* Manager)
{
    Spheroid* Spheroid_Obj = malloc(sizeof(Spheroid));
    if (Spheroid_Obj != NULL)
    {
        float SpheroidRandVel = randGetFloat(SPHEROID_MIN_VELOCITY, SPHEROID_MAX_VELOCITY);
        Coord2D Vel = { SpheroidRandVel, SpheroidRandVel };

        objInit(&Spheroid_Obj->Obj, &SpheroidVtable, Position, Vel, CalculateSpheroidBounds(Position), DefaultDir, DEFAULT_COLOR, DEFAULT_COLOR_CHANGE_DELAY, EnemySpheroid, &SpheroidCollVTable);

        SpheroidInitAnimations(Spheroid_Obj);

        Spheroid_Obj->Bounds = FieldBounds;

        Spheroid_Obj->Obj.IsActive = IsActive;

        Spheroid_Obj->FlyElapsedTime = 0.0f;
        Spheroid_Obj->EnforcerSpawnElapsedTime = 0.0f;

        // Set the Robotron character sprite sheet size
        Spheroid_Obj->SheetSize = (Coord2D){ 51.0f, 11.0f };

        Spheroid_Obj->Robotron_Obj = Robotron_Obj;

        Spheroid_Obj->Manager = Manager;
    }

    return Spheroid_Obj;
}

void SpheroidDelete(Spheroid* Spheroid_Obj)
{
    assert(Spheroid_Obj != NULL);

    objDeinit(&Spheroid_Obj->Obj);

    free(Spheroid_Obj);
}

void SpheroidDraw(Object* obj)
{
    assert(obj != NULL);
    Spheroid* Spheroid_Obj = (Spheroid*)obj;
    assert(Spheroid_Obj != NULL);

    if (obj->IsActive == false) return;

    SpriteFrame* SpheroidFrame;

    // Play spheroid spawn animation at spawn and use the last frame after the animation is done
    if (Spheroid_Obj->PlaySpawnAnim == true)
    {
        SpheroidFrame = GetCurrentFrame(Spheroid_Obj->SpawnAnim);
        if (Spheroid_Obj->SpawnAnim->CurrentFrame == Spheroid_Obj->SpawnAnim->NumFrames - 1)
            Spheroid_Obj->PlaySpawnAnim = false;
    }
    else
    {
        SpheroidFrame = GetFrame(Spheroid_Obj->SpawnAnim, 5);
    }

    assert(SpheroidFrame != NULL);

    // Enable blending to apply color tinting
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, SpheroidTexture);
    // Disable anti-aliasing
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBegin(GL_TRIANGLE_STRIP);
    {
        // Scale the sprite
        GLfloat scaledWidth = SpheroidFrame->Size.x * SPHEROID_SPRITE_SCALE;
        GLfloat scaledHeight = SpheroidFrame->Size.y * SPHEROID_SPRITE_SCALE;

        // calculate the bounding box
        GLfloat xPositionLeft = (obj->position.x - scaledWidth / 2);
        GLfloat xPositionRight = (obj->position.x + scaledWidth / 2);
        GLfloat yPositionTop = (obj->position.y - scaledHeight / 2);
        GLfloat yPositionBottom = (obj->position.y + scaledHeight / 2);

        // Calculate the UV coordinates for the sprite based on its position in the sprite sheet
        GLfloat xTextureCoord = SpheroidFrame->Position.x / Spheroid_Obj->SheetSize.x;
        GLfloat yTextureCoord = 1.0f - (SpheroidFrame->Position.y / Spheroid_Obj->SheetSize.y);
        GLfloat uWidth = SpheroidFrame->Size.x / Spheroid_Obj->SheetSize.x;
        GLfloat vHeight = SpheroidFrame->Size.y / Spheroid_Obj->SheetSize.y;

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

void SpheroidUpdate(Object* obj, uint32_t milliseconds)
{
    assert(obj != NULL);
    Spheroid* Spheroid_Obj = (Spheroid*)obj;
    assert(Spheroid_Obj != NULL && Spheroid_Obj->Manager != NULL);

    if (obj->IsActive == false || obj->IsPaused == true) return;

    SpheroidPlaySpawnAnimation(Spheroid_Obj, milliseconds);

    objSetRandomColor(obj, milliseconds);

    // Calculate bounds every frame
    obj->objBounds = CalculateSpheroidBounds(obj->position);

    // Spheroid movement
    if (Spheroid_Obj->FlyElapsedTime >= Spheroid_Obj->FlyDuration)
    {
        // Generate a new waypoint after reaching the current one
        Spheroid_Obj->Obj.velocity.x = 0;
        Spheroid_Obj->Obj.velocity.y = 0;
        Spheroid_Obj->FlyElapsedTime = 0;
        Spheroid_Obj->FlyDuration = randGetFloat(SPHEROID_MIN_FLY_DURATION, SPHEROID_MAX_FLY_DURATION);

        //Spheroid_Obj->Obj.position = Spheroid_Obj->NextWaypoint;
        Spheroid_Obj->NextWaypoint = GenerateRandomWaypoint(Spheroid_Obj->Bounds);
    }
    else 
    {
        // Move toward the waypoint with a curved motion
        Coord2D direction = {
            Spheroid_Obj->NextWaypoint.x - Spheroid_Obj->Obj.position.x,
            Spheroid_Obj->NextWaypoint.y - Spheroid_Obj->Obj.position.y
        };

        float distance = sqrtf(direction.x * direction.x + direction.y * direction.y);

        // Normalize direction
        if (distance > 2.0f) {
            direction.x /= distance;
            direction.y /= distance;
        }
        else
        {
            Spheroid_Obj->FlyElapsedTime = Spheroid_Obj->FlyDuration;
            return;
        }

        // Calculate perpendicular vector
        Coord2D perp = { -direction.y, direction.x };

        // Oscillation factor
        float oscillation = sinf(Spheroid_Obj->FlyElapsedTime * 0.005f) * 0.8f;

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
        float randVel = randGetFloat(SPHEROID_MIN_VELOCITY, SPHEROID_MAX_VELOCITY);

        Spheroid_Obj->Obj.velocity.x = finalDirection.x * randVel;
        Spheroid_Obj->Obj.velocity.y = finalDirection.y * randVel;

        // Update position
        Spheroid_Obj->Obj.position.x += Spheroid_Obj->Obj.velocity.x * milliseconds;
        Spheroid_Obj->Obj.position.y += Spheroid_Obj->Obj.velocity.y * milliseconds;

        // Increment elapsed time
        Spheroid_Obj->FlyElapsedTime += milliseconds;
    }

    // Spheroid enforcer spawning
    if (Spheroid_Obj->EnforcerSpawnElapsedTime >= Spheroid_Obj->EnforcerSpawnDelay)
    {
        Spheroid_Obj->EnforcerSpawnElapsedTime = 0.0f;

        // Spawn enforcer
        ProjectileManagerSpawnEnforcer(Spheroid_Obj->Manager, Spheroid_Obj->Obj.position);
    }
    else
    {
        Spheroid_Obj->EnforcerSpawnElapsedTime += milliseconds;
    }
}

void SpheroidColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType)
{
}