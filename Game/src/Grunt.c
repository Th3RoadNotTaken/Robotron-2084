#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <gl/GLU.h>
#include "SOIL.h"

#include "Grunt.h"
#include "random.h"
#include "stdbool.h"
#include "math.h"

static const char GruntSprite[] = "asset/Grunt_White.png";

static GLuint GruntTexture = 0;

static void GruntUpdate(Object* obj, uint32_t milliseconds);
static void GruntDraw(Object* obj);
static void GruntColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType);

static ObjVtable GruntVtable = {
    GruntDraw,
    GruntUpdate
};
static ObjCollisionVtable GruntCollVTable = {
    GruntColl
};

/// @brief one time initialization of textures
void GruntInitTextures()
{
	if (GruntTexture == 0)
	{
		/*GruntTexture = SOIL_load_OGL_texture(GruntSprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		assert(GruntTexture != 0);*/
        GruntTexture = SOIL_load_OGL_texture(GruntSprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
            SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB);
        assert(GruntTexture != 0);
	}
}

void GruntInitAnimations(Grunt* Grunt_Obj)
{
    assert(Grunt_Obj != NULL);

    // Running up animations
    SpriteFrame* RunSpriteFrames = malloc(sizeof(SpriteFrame) * GRUNT_RUNANIM_TYPES);
    assert(RunSpriteFrames != NULL);

    RunSpriteFrames[0].Position = (Coord2D){ GRUNT_RUN_1_POS_X, GRUNT_RUN_POS_Y };
    RunSpriteFrames[0].Size = (Coord2D){ GRUNT_RUN_SIZE_X , GRUNT_RUN_SIZE_Y };

    RunSpriteFrames[1].Position = (Coord2D){ GRUNT_RUN_2_POS_X, GRUNT_RUN_POS_Y };
    RunSpriteFrames[1].Size = (Coord2D){ GRUNT_RUN_SIZE_X , GRUNT_RUN_SIZE_Y };

    RunSpriteFrames[2].Position = (Coord2D){ GRUNT_RUN_3_POS_X, GRUNT_RUN_POS_Y };
    RunSpriteFrames[2].Size = (Coord2D){ GRUNT_RUN_SIZE_X , GRUNT_RUN_SIZE_Y };

    Grunt_Obj->RunAnim = InitAnimation(RunSpriteFrames, GRUNT_RUNANIM_TYPES, 0, GRUNT_FRAME_DUR);
}

Bounds2D CalculateGruntBounds(Coord2D Position)
{
    float scaledWidth = GRUNT_BOUND_X * GRUNT_SPRITE_SCALE;
    float scaledHeight = GRUNT_BOUND_Y * GRUNT_SPRITE_SCALE;

    // calculate the bounding box
    float xPositionLeft = (Position.x - scaledWidth / 2);
    float xPositionRight = (Position.x + scaledWidth / 2);
    float yPositionTop = (Position.y - scaledHeight / 2);
    float yPositionBottom = (Position.y + scaledHeight / 2);

    return (Bounds2D) { { xPositionLeft, yPositionTop }, { xPositionRight, yPositionBottom } };
}

Grunt* GruntNew(Coord2D Position, Bounds2D FieldBounds, Robotron* Robotron_Obj, bool IsActive)
{
    Grunt* Grunt_Obj = malloc(sizeof(Grunt));
    if (Grunt_Obj != NULL)
    {
        float GruntRandVel = randGetFloat(GRUNT_MIN_VELOCITY, GRUNT_MAX_VELOCITY);
        Coord2D Vel = { GruntRandVel, GruntRandVel };
        objInit(&Grunt_Obj->Obj, &GruntVtable, Position, Vel, CalculateGruntBounds(Position), DefaultDir, DEFAULT_COLOR, DEFAULT_COLOR_CHANGE_DELAY, EnemyGrunt, &GruntCollVTable);

        GruntInitAnimations(Grunt_Obj);

        Grunt_Obj->Bounds = FieldBounds;

        Grunt_Obj->Obj.IsActive = IsActive;

        Grunt_Obj->StopElapsedTime = 0;

        // Set the Robotron character sprite sheet size
        Grunt_Obj->SheetSize = (Coord2D){ 39.0f, 13.0f };

        Grunt_Obj->Robotron_Obj = Robotron_Obj;
    }

    return Grunt_Obj;
}

void GruntDelete(Grunt* Grunt_Obj)
{
    assert(Grunt_Obj != NULL);

    objDeinit(&Grunt_Obj->Obj);

    free(Grunt_Obj);
}

void GruntDraw(Object* obj)
{
    assert(obj != NULL);
    Grunt* Grunt_Obj = (Grunt*)obj;
    assert(Grunt_Obj != NULL);

    if (obj->IsActive == false) return;

    // Get the current frame of the selected animation
    SpriteFrame* CurrentFrame = GetCurrentFrame(Grunt_Obj->RunAnim);

    assert(CurrentFrame != NULL);

    // Enable blending to apply color tinting
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, GruntTexture);
    // Disable anti-aliasing
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBegin(GL_TRIANGLE_STRIP);
    {
        // Scale the sprite
        GLfloat scaledWidth = CurrentFrame->Size.x * GRUNT_SPRITE_SCALE;
        GLfloat scaledHeight = CurrentFrame->Size.y * GRUNT_SPRITE_SCALE;

        // calculate the bounding box
        GLfloat xPositionLeft = (obj->position.x - scaledWidth / 2);
        GLfloat xPositionRight = (obj->position.x + scaledWidth / 2);
        GLfloat yPositionTop = (obj->position.y - scaledHeight / 2);
        GLfloat yPositionBottom = (obj->position.y + scaledHeight / 2);

        // Calculate the UV coordinates for the sprite based on its position in the sprite sheet
        GLfloat xTextureCoord = CurrentFrame->Position.x / Grunt_Obj->SheetSize.x;
        GLfloat yTextureCoord = 1.0f - (CurrentFrame->Position.y / Grunt_Obj->SheetSize.y);
        GLfloat uWidth = CurrentFrame->Size.x / Grunt_Obj->SheetSize.x;
        GLfloat vHeight = CurrentFrame->Size.y / Grunt_Obj->SheetSize.y;

        const float BG_DEPTH = -0.99f;

        GLubyte r, g, b;

        r = ((obj->color >> 16) & 0xFF);
        g = ((obj->color >> 8) & 0xFF);
        b = ((obj->color >> 0) & 0xFF);

        glColor4ub(r, g, b, 0xFF);

        // TL
        glTexCoord2f(xTextureCoord, yTextureCoord);
        glVertex3f(xPositionLeft, yPositionTop, BG_DEPTH);

        glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);

        // BL
        glTexCoord2f(xTextureCoord, yTextureCoord - vHeight);
        glVertex3f(xPositionLeft, yPositionBottom, BG_DEPTH);

        glColor4ub(r, g, b, 0xFF);

        // TR
        glTexCoord2f(xTextureCoord + uWidth, yTextureCoord);
        glVertex3f(xPositionRight, yPositionTop, BG_DEPTH);

        glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);

        // BR
        glTexCoord2f(xTextureCoord + uWidth, yTextureCoord - vHeight);
        glVertex3f(xPositionRight, yPositionBottom, BG_DEPTH);

    }
    glEnd();
}

void GruntUpdate(Object* obj, uint32_t milliseconds)
{
    assert(obj != NULL);
    Grunt* Grunt_Obj = (Grunt*)obj;
    assert(Grunt_Obj != NULL);

    if (obj->IsActive == false || obj->IsPaused == true) return;
    
    PlayAnimation(Grunt_Obj->RunAnim, milliseconds);

    objSetRandomColor(obj, milliseconds);

    // Calculate bounds every frame
    obj->objBounds = CalculateGruntBounds(obj->position);

    // AI to chase the player

    Coord2D Robotron_Pos = Grunt_Obj->Robotron_Obj->Obj.position;
    Coord2D Grunt_Pos = obj->position;

    if (Grunt_Obj->MoveElapsedTime >= Grunt_Obj->MoveDuration)
    {
        if (Grunt_Obj->StopElapsedTime >= Grunt_Obj->StopDuration)
        {
            Grunt_Obj->StopElapsedTime = 0.0f;
            Grunt_Obj->MoveElapsedTime = 0.0f;
            float randDuration = randGetFloat(GRUNT_MOVE_MIN_DURATION, GRUNT_MOVE_MAX_DURATION);
            Grunt_Obj->MoveDuration = randDuration;
            randDuration = randGetFloat(GRUNT_STOP_MIN_DURATION, GRUNT_STOP_MAX_DURATION);
            Grunt_Obj->StopDuration = randDuration;
        }
        else
        {
            Grunt_Obj->StopElapsedTime += milliseconds;
        }
    }
    else
    {
        // Neither x or y coordinate matching with player character
        if (fabs(Grunt_Pos.x - Robotron_Pos.x) > DELTA_MOVEMENT && fabs(Grunt_Pos.y - Robotron_Pos.y) > DELTA_MOVEMENT)
        {
            if (Robotron_Pos.x < Grunt_Pos.x)
                obj->position.x -= obj->velocity.x * milliseconds * GRUNT_CHAR_DIAGONAL_SPEED;
            else
                obj->position.x += obj->velocity.x * milliseconds * GRUNT_CHAR_DIAGONAL_SPEED;

            if (Robotron_Pos.y < Grunt_Pos.y)
                obj->position.y -= obj->velocity.y * milliseconds * GRUNT_CHAR_DIAGONAL_SPEED;
            else
                obj->position.y += obj->velocity.y * milliseconds * GRUNT_CHAR_DIAGONAL_SPEED;
        }
        else if (fabs(Grunt_Pos.x - Robotron_Pos.x) <= DELTA_MOVEMENT)
        {
            if (Robotron_Pos.y < Grunt_Pos.y)
                obj->position.y -= obj->velocity.y * milliseconds;
            else
                obj->position.y += obj->velocity.y * milliseconds;
        }
        else if (fabs(Grunt_Pos.y - Robotron_Pos.y) <= DELTA_MOVEMENT)
        {
            if (Robotron_Pos.x < Grunt_Pos.x)
                obj->position.x -= obj->velocity.x * milliseconds;
            else
                obj->position.x += obj->velocity.x * milliseconds;
        }

        Grunt_Obj->MoveElapsedTime += milliseconds;
    }
}

void GruntColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType)
{
    assert(SourceObject != NULL && TargetObject != NULL);

    if (SourceObject->IsPaused == true) return;

    if (SourceObject->IsActive == false || TargetObject->IsActive == false) return;

    float SrcRightSide = SourceObject->objBounds.botRight.x;
    float SrcLeftSide = SourceObject->objBounds.topLeft.x;
    float SrcTopSide = SourceObject->objBounds.topLeft.y;
    float SrcBottomSide = SourceObject->objBounds.botRight.y;

    float TargetRightSide = TargetObject->objBounds.botRight.x;
    float TargetLeftSide = TargetObject->objBounds.topLeft.x;
    float TargetTopSide = TargetObject->objBounds.topLeft.y;
    float TargetBottomSide = TargetObject->objBounds.botRight.y;

    // Check if the bounding boxes overlap on both axes
    bool isColliding = !(SrcRightSide < TargetLeftSide || // Source is to the left of Target
                         SrcLeftSide > TargetRightSide || // Source is to the right of Target
                         SrcTopSide > TargetBottomSide || // Source is above Target
                         SrcBottomSide < TargetTopSide);  // Source is below Target

    if (isColliding == true)
    {
        if (SourceObjType == TargetObjectType)
        {
            /*SourceObject->position.x += randGetFloat(1.5f, 2.0f);
            SourceObject->position.y += randGetFloat(1.5f, 2.0f);
            TargetObject->position.x -= randGetFloat(1.5f, 2.0f);
            TargetObject->position.y -= randGetFloat(1.5f, 2.0f);*/
        }
    }
}