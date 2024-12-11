#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <gl/GLU.h>
#include "SOIL.h"

#include "Enforcer.h"
#include "random.h"
#include "stdbool.h"
#include "math.h"

static const char EnforcerSprite[] = "asset/Enforcer_White.png";

static GLuint EnforcerTexture = 0;
static EnforcerBoundaryCollide BoundaryCollCB = NULL;

static void EnforcerUpdate(Object* obj, uint32_t milliseconds);
static void EnforcerDraw(Object* obj);
static void EnforcerColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType);

static ObjVtable EnforcerVtable = {
	EnforcerDraw,
	EnforcerUpdate
};
static ObjCollisionVtable EnforcerCollVTable = {
	EnforcerColl
};

void EnforcerSetCollideCB(EnforcerBoundaryCollide cb)
{
    BoundaryCollCB = cb;
}

void EnforcerClearCollideCB()
{
    BoundaryCollCB = NULL;
}

void EnforcerInitTextures()
{
	if (EnforcerTexture == 0)
	{
        EnforcerTexture = SOIL_load_OGL_texture(EnforcerSprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
            SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB);
        assert(EnforcerTexture != 0);
	}
}

void EnforcerInitAnimations(Enforcer* Enforcer_Obj)
{
    assert(Enforcer_Obj != NULL);

    // Running left animations
    SpriteFrame* FlySpriteFrames = malloc(sizeof(SpriteFrame) * ENFORCER_FLY_ANIMS);
    assert(FlySpriteFrames != NULL);

    FlySpriteFrames[0].Position = (Coord2D){ ENFORCER_FLY_1_POS_X, ENFORCER_FLY_POS_Y };
    FlySpriteFrames[0].Size = (Coord2D){ ENFORCER_FLY_SIZE_X , ENFORCER_FLY_SIZE_Y };

    FlySpriteFrames[1].Position = (Coord2D){ ENFORCER_FLY_2_POS_X, ENFORCER_FLY_POS_Y };
    FlySpriteFrames[1].Size = (Coord2D){ ENFORCER_FLY_SIZE_X , ENFORCER_FLY_SIZE_Y };

    FlySpriteFrames[2].Position = (Coord2D){ ENFORCER_FLY_3_POS_X, ENFORCER_FLY_POS_Y };
    FlySpriteFrames[2].Size = (Coord2D){ ENFORCER_FLY_SIZE_X , ENFORCER_FLY_SIZE_Y };

    FlySpriteFrames[3].Position = (Coord2D){ ENFORCER_FLY_4_POS_X, ENFORCER_FLY_POS_Y };
    FlySpriteFrames[3].Size = (Coord2D){ ENFORCER_FLY_SIZE_X , ENFORCER_FLY_SIZE_Y };

    Enforcer_Obj->FlyAnim = InitAnimation(FlySpriteFrames, ENFORCER_FLY_ANIMS, 0, ENFORCER_FRAME_DUR);
}

Bounds2D CalculateEnforcerBounds(Coord2D Position)
{
    float scaledWidth = ENFORCER_BOUND_X * ENFORCER_SPRITE_SCALE;
    float scaledHeight = ENFORCER_BOUND_Y * ENFORCER_SPRITE_SCALE;

    // calculate the bounding box
    float xPositionLeft = (Position.x - scaledWidth / 2);
    float xPositionRight = (Position.x + scaledWidth / 2);
    float yPositionTop = (Position.y - scaledHeight / 2);
    float yPositionBottom = (Position.y + scaledHeight / 2);

    return (Bounds2D) { { xPositionLeft, yPositionTop }, { xPositionRight, yPositionBottom } };
}

Enforcer* EnforcerNew(Coord2D Position, Bounds2D FieldBounds, Robotron* Robotron_Obj, bool IsActive)
{
    Enforcer* Enforcer_Obj = malloc(sizeof(Enforcer));
    if (Enforcer_Obj != NULL)
    {
        float EnforcerRandVel = randGetFloat(ENFORCER_MIN_VELOCITY, ENFORCER_MAX_VELOCITY);
        Coord2D Vel = { EnforcerRandVel, EnforcerRandVel };

        objInit(&Enforcer_Obj->Obj, &EnforcerVtable, Position, Vel, CalculateEnforcerBounds(Position), DefaultDir, DEFAULT_COLOR, DEFAULT_COLOR_CHANGE_DELAY, EnemyEnforcer, &EnforcerCollVTable);

        Enforcer_Obj->Obj.colorChangeDelay = 200;

        EnforcerInitAnimations(Enforcer_Obj);

        Enforcer_Obj->Bounds = FieldBounds;

        Enforcer_Obj->Obj.IsActive = IsActive;

        // Set the enforcer character sprite sheet size
        Enforcer_Obj->SheetSize = (Coord2D){ 30.0f, 7.0f };

        Enforcer_Obj->Robotron_Obj = Robotron_Obj;
    }

    return Enforcer_Obj;
}

void EnforcerDelete(Enforcer* Enforcer_Obj)
{
    assert(Enforcer_Obj != NULL);

    objDeinit(&Enforcer_Obj->Obj);

    free(Enforcer_Obj);
}

void EnforcerDraw(Object* obj)
{
    assert(obj != NULL);
    Enforcer* Enforcer_Obj = (Enforcer*)obj;
    assert(Enforcer_Obj != NULL);

    if (obj->IsActive == false) return;

    // Get the current frame of the fly animation
    SpriteFrame* CurrentFrame = GetCurrentFrame(Enforcer_Obj->FlyAnim);

    assert(CurrentFrame != NULL);

    // Scale the sprite
    GLfloat scaledWidth = CurrentFrame->Size.x * ENFORCER_SPRITE_SCALE;
    GLfloat scaledHeight = CurrentFrame->Size.y * ENFORCER_SPRITE_SCALE;

    // calculate the bounding box
    GLfloat xPositionLeft = (obj->position.x - scaledWidth / 2);
    GLfloat xPositionRight = (obj->position.x + scaledWidth / 2);
    GLfloat yPositionTop = (obj->position.y - scaledHeight / 2);
    GLfloat yPositionBottom = (obj->position.y + scaledHeight / 2);

    // Calculate the UV coordinates for the sprite based on its position in the sprite sheet
    GLfloat xTextureCoord = CurrentFrame->Position.x / Enforcer_Obj->SheetSize.x;
    GLfloat yTextureCoord = 1.0f - (CurrentFrame->Position.y / Enforcer_Obj->SheetSize.y);
    GLfloat uWidth = CurrentFrame->Size.x / Enforcer_Obj->SheetSize.x;
    GLfloat vHeight = CurrentFrame->Size.y / Enforcer_Obj->SheetSize.y;

    const float BG_DEPTH = -0.99f;

    // Enable blending to apply color tinting
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, EnforcerTexture);
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

void EnforcerUpdate(Object* obj, uint32_t milliseconds)
{
    assert(obj != NULL);
    Enforcer* Enforcer_Obj = (Enforcer*)obj;
    assert(Enforcer_Obj != NULL);

    if (obj->IsActive == false || obj->IsPaused == true) return;

    EnforcerCheckBoundaryColl(Enforcer_Obj);

    objSetRandomColor(obj, milliseconds);

    PlayAnimation(Enforcer_Obj->FlyAnim, milliseconds);

    // Calculate bounds every frame
    obj->objBounds = CalculateEnforcerBounds(obj->position);

    // Move towards the initial player position
    if (Enforcer_Obj->VelocityInitialized == false)
    {
        Coord2D direction = {
            Enforcer_Obj->TargetPoint.x - Enforcer_Obj->Obj.position.x,
            Enforcer_Obj->TargetPoint.y - Enforcer_Obj->Obj.position.y
        };

        float distance = sqrtf(direction.x * direction.x + direction.y * direction.y);
        // Normalize the distance
        if (distance > 0.0f) {
            direction.x /= distance;
            direction.y /= distance;
        }

        // Update velocity
        float randVel = randGetFloat(ENFORCER_MIN_VELOCITY, ENFORCER_MAX_VELOCITY);

        Enforcer_Obj->Obj.velocity.x = direction.x * randVel;
        Enforcer_Obj->Obj.velocity.y = direction.y * randVel;

        Enforcer_Obj->VelocityInitialized = true;
    }

    // Update position
    Enforcer_Obj->Obj.position.x += Enforcer_Obj->Obj.velocity.x * milliseconds;
    Enforcer_Obj->Obj.position.y += Enforcer_Obj->Obj.velocity.y * milliseconds;
}

void EnforcerCheckBoundaryColl(Enforcer* Enforcer_Obj)
{
    float rightSide = Enforcer_Obj->Bounds.botRight.x;
    float leftSide = Enforcer_Obj->Bounds.topLeft.x;
    float topSide = Enforcer_Obj->Bounds.topLeft.y;
    float bottomSide = Enforcer_Obj->Bounds.botRight.y;

    if (Enforcer_Obj->Obj.position.x <= leftSide ||
        Enforcer_Obj->Obj.position.x >= rightSide ||
        Enforcer_Obj->Obj.position.y >= bottomSide ||
        Enforcer_Obj->Obj.position.y <= topSide)
    {
        BoundaryCollCB(&Enforcer_Obj->Obj);
    }
}

void EnforcerColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType)
{
    
}