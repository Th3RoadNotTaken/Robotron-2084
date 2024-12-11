#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <gl/GLU.h>
#include "SOIL.h"

#include "Explosion.h"
#include "random.h"
#include "stdbool.h"

static const char ExplosionSprite[] = "asset/AllExplosions.png";

static GLuint ExplosionTexture = 0;
static AnimComplete animCompleteCB = NULL;

static void ExplosionUpdate(Object* obj, uint32_t milliseconds);
static void ExplosionDraw(Object* obj);
static void ExplosionColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType);

static ObjVtable ExplosionVtable = {
    ExplosionDraw,
    ExplosionUpdate
};
static ObjCollisionVtable ExplosionCollVTable = {
    ExplosionColl
};

void AnimCompleteSetCollideCB(AnimComplete cb)
{
    animCompleteCB = cb;
}

void AnimCompleteClearCollideCB()
{
    animCompleteCB = NULL;
}

void ExplosionInitTextures()
{
    if (ExplosionTexture == 0)
    {
        ExplosionTexture = SOIL_load_OGL_texture(ExplosionSprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
            SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB);
        assert(ExplosionTexture != 0);
    }
}

void ExplosionInitAnimations(Explosion* Explosion_Obj)
{
    assert(Explosion_Obj != NULL);

    // Initialize animations
    Explosion_Obj->ExplosionAnimations = malloc(sizeof(SpriteAnimation*) * EXP_ANIM_TYPES);
    assert(Explosion_Obj->ExplosionAnimations != NULL);

    for (int i = 0; i < EXP_ANIM_TYPES; i++)
    {
        SpriteFrame* Frames = malloc(sizeof(SpriteFrame) * EXP_ANIM_TYPES);
        assert(Frames != NULL);

        float YCoord = 0;
        switch (i)
        {
        case 0: YCoord = EXP_TYPE1_POS_Y; break;
        case 1: YCoord = EXP_TYPE2_POS_Y; break;
        case 2: YCoord = EXP_TYPE3_POS_Y; break;
        case 3: YCoord = EXP_TYPE4_POS_Y; break;
        case 4: YCoord = EXP_TYPE5_POS_Y; break;
        case 5: YCoord = EXP_TYPE6_POS_Y; break;
        case 6: YCoord = EXP_TYPE7_POS_Y; break;
        case 7: YCoord = EXP_TYPE8_POS_Y; break;
        case 8: YCoord = EXP_TYPE9_POS_Y; break;
        }
        Frames[0].Position = (Coord2D){ EXP_1_POS_X, YCoord };
        Frames[0].Size = (Coord2D){ EXP_SIZE_X , EXP_SIZE_Y };

        Frames[1].Position = (Coord2D){ EXP_2_POS_X, YCoord };
        Frames[1].Size = (Coord2D){ EXP_SIZE_X , EXP_SIZE_Y };

        Frames[2].Position = (Coord2D){ EXP_3_POS_X, YCoord };
        Frames[2].Size = (Coord2D){ EXP_SIZE_X , EXP_SIZE_Y };

        Frames[3].Position = (Coord2D){ EXP_4_POS_X, YCoord };
        Frames[3].Size = (Coord2D){ EXP_SIZE_X , EXP_SIZE_Y };

        Frames[4].Position = (Coord2D){ EXP_5_POS_X, YCoord };
        Frames[4].Size = (Coord2D){ EXP_SIZE_X , EXP_SIZE_Y };

        Frames[5].Position = (Coord2D){ EXP_6_POS_X, YCoord };
        Frames[5].Size = (Coord2D){ EXP_SIZE_X , EXP_SIZE_Y };

        Frames[6].Position = (Coord2D){ EXP_7_POS_X, YCoord };
        Frames[6].Size = (Coord2D){ EXP_SIZE_X , EXP_SIZE_Y };

        Frames[7].Position = (Coord2D){ EXP_8_POS_X, YCoord };
        Frames[7].Size = (Coord2D){ EXP_SIZE_X , EXP_SIZE_Y };

        Frames[8].Position = (Coord2D){ EXP_9_POS_X, YCoord };
        Frames[8].Size = (Coord2D){ EXP_SIZE_X , EXP_SIZE_Y };

        Explosion_Obj->ExplosionAnimations[i] = InitAnimation(Frames, EXP_ANIM_TYPES, 0, ANIM_FRAME_DUR);
    }
}

Explosion* ExplosionNew(Bounds2D FieldBounds, bool IsActive, Coord2D Position)
{
    Explosion* Explosion_Obj = malloc(sizeof(Explosion));
    if (Explosion_Obj != NULL)
    {
        objInitDefault(&Explosion_Obj->Obj, &ExplosionVtable, (Coord2D) { 0, 0 }, (Coord2D) { 0, 0 }, UI, & ExplosionCollVTable);

        ExplosionInitAnimations(Explosion_Obj);

        Explosion_Obj->Bounds = FieldBounds;

        Explosion_Obj->Obj.IsActive = IsActive;
        Explosion_Obj->ExplosionAnimIndex = 0;
        Explosion_Obj->shouldPlayAnim = false;

        // Set the explosion sprite sheet size
        Explosion_Obj->ExplosionSheetSize = (Coord2D){ 576.0f, 576.0f };
    }

    return Explosion_Obj;
}

void ExplosionDelete(Explosion* Explosion_Obj)
{
    assert(Explosion_Obj != NULL);

    objDeinit(&Explosion_Obj->Obj);

    free(Explosion_Obj);
}

void ExplosionDraw(Object* obj)
{
    assert(obj != NULL);
    Explosion* Explosion_Obj = (Explosion*)obj;
    assert(Explosion_Obj != NULL);

    if (Explosion_Obj->shouldPlayAnim == false) return;

    // Get current frame
    SpriteFrame* CurrentFrame = GetCurrentFrame(Explosion_Obj->ExplosionAnimations[Explosion_Obj->ExplosionAnimIndex]);

    // Scale the explosion sprite
    GLfloat scaledWidth = (GLfloat)(CurrentFrame->Size.x * EXP_SPRITE_SCALE);
    GLfloat scaledHeight = (GLfloat)(CurrentFrame->Size.y * EXP_SPRITE_SCALE);

    // Calculate the bounding box
    GLfloat xPositionLeft = (obj->position.x - scaledWidth / 2);
    GLfloat xPositionRight = (obj->position.x + scaledWidth / 2);
    GLfloat yPositionTop = (obj->position.y - scaledHeight / 2);
    GLfloat yPositionBottom = (obj->position.y + scaledHeight / 2);

    // Calculate the UV coordinates for the sprite based on its position in the sprite sheet
    GLfloat xTextureCoord = CurrentFrame->Position.x / Explosion_Obj->ExplosionSheetSize.x;
    GLfloat yTextureCoord = 1.0f - (CurrentFrame->Position.y / Explosion_Obj->ExplosionSheetSize.y);
    GLfloat uWidth = CurrentFrame->Size.x / Explosion_Obj->ExplosionSheetSize.x;
    GLfloat vHeight = CurrentFrame->Size.y / Explosion_Obj->ExplosionSheetSize.y;

    const float BG_DEPTH = -0.99f;

    // Enable blending to apply color tinting
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ExplosionTexture);
    // Disable anti-aliasing
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBegin(GL_TRIANGLE_STRIP);
    {
        glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);

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

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void ExplosionUpdate(Object* obj, uint32_t milliseconds)
{
    assert(obj != NULL);
    Explosion* Explosion_Obj = (Explosion*)obj;
    assert(Explosion_Obj != NULL);

    if (obj->IsActive == false || obj->IsPaused == true) return;

    bool shouldPlayAnim = PlayAnimationOneShot(Explosion_Obj->ExplosionAnimations[Explosion_Obj->ExplosionAnimIndex], milliseconds);

    if (shouldPlayAnim == false)
    {
        Explosion_Obj->shouldPlayAnim = false;
        animCompleteCB(obj);
    }
}

void ExplosionColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType)
{
}