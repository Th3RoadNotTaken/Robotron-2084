#include <Windows.h>
#include <stdlib.h>
#include <assert.h>
#include <gl/GLU.h>
#include "SOIL.h"

#include "Hulk.h"
#include "random.h"
#include "stdbool.h"
#include "math.h"
#include "Spheroid.h"

static const char HulkSprite[] = "asset/Hulk_White.png";

static GLuint HulkTexture = 0;

static void HulkUpdate(Object* obj, uint32_t milliseconds);
static void HulkDraw(Object* obj);
static void HulkColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType);

static ObjVtable HulkVtable = {
    HulkDraw,
    HulkUpdate
};
static ObjCollisionVtable HulkCollVTable = {
    HulkColl
};

/// @brief one time initialization of textures
void HulkInitTextures()
{
	if (HulkTexture == 0)
	{
		/*HulkTexture = SOIL_load_OGL_texture(HulkSprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		assert(HulkTexture != 0);*/
        HulkTexture = SOIL_load_OGL_texture(HulkSprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
            SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB);
        assert(HulkTexture != 0);
	}
}

void HulkInitAnimations(Hulk* Hulk_Obj)
{
    assert(Hulk_Obj != NULL);

    // Running left animations
    SpriteFrame* RunLeftSpriteFrames = malloc(sizeof(SpriteFrame) * HULK_RUNANIM_TYPES);
    assert(RunLeftSpriteFrames != NULL);

    RunLeftSpriteFrames[0].Position = (Coord2D){ HULK_RL_1_POS_X, HULK_RL_1_POS_Y };
    RunLeftSpriteFrames[0].Size = (Coord2D){ HULK_RL_SIZE_X , HULK_RL_SIZE_Y };

    RunLeftSpriteFrames[1].Position = (Coord2D){ HULK_RL_2_POS_X, HULK_RL_2_POS_Y };
    RunLeftSpriteFrames[1].Size = (Coord2D){ HULK_RL_SIZE_X , HULK_RL_SIZE_Y };

    RunLeftSpriteFrames[2].Position = (Coord2D){ HULK_RL_3_POS_X, HULK_RL_3_POS_Y };
    RunLeftSpriteFrames[2].Size = (Coord2D){ HULK_RL_SIZE_X , HULK_RL_SIZE_Y };

    Hulk_Obj->RunLeftAnim = InitAnimation(RunLeftSpriteFrames, HULK_RUNANIM_TYPES, 0, HULK_FRAME_DUR);

    // Running down animations
    SpriteFrame* RunDownSpriteFrames = malloc(sizeof(SpriteFrame) * HULK_RUNANIM_TYPES);
    assert(RunDownSpriteFrames != NULL);

    RunDownSpriteFrames[0].Position = (Coord2D){ HULK_RD_1_POS_X, HULK_RD_1_POS_Y };
    RunDownSpriteFrames[0].Size = (Coord2D){ HULK_RD_SIZE_X , HULK_RD_SIZE_Y };

    RunDownSpriteFrames[1].Position = (Coord2D){ HULK_RD_2_POS_X, HULK_RD_2_POS_Y };
    RunDownSpriteFrames[1].Size = (Coord2D){ HULK_RD_SIZE_X , HULK_RD_SIZE_Y };

    RunDownSpriteFrames[2].Position = (Coord2D){ HULK_RD_3_POS_X, HULK_RD_3_POS_Y };
    RunDownSpriteFrames[2].Size = (Coord2D){ HULK_RD_SIZE_X , HULK_RD_SIZE_Y };

    Hulk_Obj->RunDownAnim = InitAnimation(RunDownSpriteFrames, HULK_RUNANIM_TYPES, 0, HULK_FRAME_DUR);

    // Running right animations
    SpriteFrame* RunRightSpriteFrames = malloc(sizeof(SpriteFrame) * HULK_RUNANIM_TYPES);
    assert(RunRightSpriteFrames != NULL);

    RunRightSpriteFrames[0].Position = (Coord2D){ HULK_RR_1_POS_X, HULK_RR_1_POS_Y };
    RunRightSpriteFrames[0].Size = (Coord2D){ HULK_RR_SIZE_X , HULK_RR_SIZE_Y };

    RunRightSpriteFrames[1].Position = (Coord2D){ HULK_RR_2_POS_X, HULK_RR_2_POS_Y };
    RunRightSpriteFrames[1].Size = (Coord2D){ HULK_RR_SIZE_X , HULK_RR_SIZE_Y };

    RunRightSpriteFrames[2].Position = (Coord2D){ HULK_RR_3_POS_X, HULK_RR_3_POS_Y };
    RunRightSpriteFrames[2].Size = (Coord2D){ HULK_RR_SIZE_X , HULK_RR_SIZE_Y };

    Hulk_Obj->RunRightAnim = InitAnimation(RunRightSpriteFrames, HULK_RUNANIM_TYPES, 0, HULK_FRAME_DUR);
}

Bounds2D CalculateHulkBounds(Coord2D Position)
{
    float scaledWidth = HULK_BOUND_X * HULK_SPRITE_SCALE;
    float scaledHeight = HULK_BOUND_Y * HULK_SPRITE_SCALE;

    // calculate the bounding box
    float xPositionLeft = (Position.x - scaledWidth / 2);
    float xPositionRight = (Position.x + scaledWidth / 2);
    float yPositionTop = (Position.y - scaledHeight / 2);
    float yPositionBottom = (Position.y + scaledHeight / 2);

    return (Bounds2D) { { xPositionLeft, yPositionTop }, { xPositionRight, yPositionBottom } };
}

Hulk* HulkNew(Coord2D Position, Bounds2D FieldBounds, Robotron* Robotron_Obj, bool IsActive)
{
    Hulk* Hulk_Obj = malloc(sizeof(Hulk));
    if (Hulk_Obj != NULL)
    {
        float HulkRandVel = randGetFloat(HULK_MIN_VELOCITY, HULK_MAX_VELOCITY);
        Coord2D Vel = { HulkRandVel, HulkRandVel };

        objInit(&Hulk_Obj->Obj, &HulkVtable, Position, Vel, CalculateHulkBounds(Position), DefaultDir, DEFAULT_COLOR, DEFAULT_COLOR_CHANGE_DELAY, EnemyHulk, &HulkCollVTable);

        HulkInitAnimations(Hulk_Obj);

        Hulk_Obj->Bounds = FieldBounds;

        Hulk_Obj->Obj.IsActive = IsActive;

        Hulk_Obj->WalkElapsedTime = 0.0f;

        // Set the Robotron character sprite sheet size
        Hulk_Obj->SheetSize = (Coord2D){ 160.0f, 16.0f };

        Hulk_Obj->Robotron_Obj = Robotron_Obj;
    }

    return Hulk_Obj;
}

void HulkDelete(Hulk* Hulk_Obj)
{
    assert(Hulk_Obj != NULL);

    objDeinit(&Hulk_Obj->Obj);

    free(Hulk_Obj);
}

void HulkDraw(Object* obj)
{
    assert(obj != NULL);
    Hulk* Hulk_Obj = (Hulk*)obj;
    assert(Hulk_Obj != NULL);

    if (obj->IsActive == false) return;

    // Choose the correct animation based on direction
    SpriteAnimation* CurrentAnimation;
    switch (obj->direction)
    {
    case Up: CurrentAnimation = Hulk_Obj->RunDownAnim; break;
    case Down: CurrentAnimation = Hulk_Obj->RunDownAnim; break;
    case Left: CurrentAnimation = Hulk_Obj->RunLeftAnim; break;
    case Right: CurrentAnimation = Hulk_Obj->RunRightAnim; break;
    default: CurrentAnimation = Hulk_Obj->RunDownAnim; break; // default to Down if none
    }

    // Get the current frame of the selected animation
    SpriteFrame* CurrentFrame = GetCurrentFrame(CurrentAnimation);

    assert(CurrentFrame != NULL);

    // Enable blending to apply color tinting
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, HulkTexture);
    // Disable anti-aliasing
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBegin(GL_TRIANGLE_STRIP);
    {
        // Scale the sprite
        GLfloat scaledWidth = CurrentFrame->Size.x * HULK_SPRITE_SCALE;
        GLfloat scaledHeight = CurrentFrame->Size.y * HULK_SPRITE_SCALE;

        // calculate the bounding box
        GLfloat xPositionLeft = (obj->position.x - scaledWidth / 2);
        GLfloat xPositionRight = (obj->position.x + scaledWidth / 2);
        GLfloat yPositionTop = (obj->position.y - scaledHeight / 2);
        GLfloat yPositionBottom = (obj->position.y + scaledHeight / 2);

        // Calculate the UV coordinates for the sprite based on its position in the sprite sheet
        GLfloat xTextureCoord = CurrentFrame->Position.x / Hulk_Obj->SheetSize.x;
        GLfloat yTextureCoord = 1.0f - (CurrentFrame->Position.y / Hulk_Obj->SheetSize.y);
        GLfloat uWidth = CurrentFrame->Size.x / Hulk_Obj->SheetSize.x;
        GLfloat vHeight = CurrentFrame->Size.y / Hulk_Obj->SheetSize.y;

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

    //glDisable(GL_TEXTURE_2D); 
    //glDisable(GL_BLEND);      

    //float scaledWidth = HULK_BOUND_X * HULK_SPRITE_SCALE;
    //float scaledHeight = HULK_BOUND_Y * HULK_SPRITE_SCALE;

    //float halfHeight = scaledHeight / 2.0f;
    //float halfWidth = scaledWidth / 2.0f;
    //float left = obj->position.x - halfWidth;
    //float right = obj->position.x + halfWidth;
    //float top = obj->position.y - halfHeight;
    //float bottom = obj->position.y + halfHeight;

    //glLineWidth(2.0f);        // Thicker lines for debugging
    //glColor3f(1.0f, 0.0f, 0.0f); // Yellow color for visibility

    //glBegin(GL_LINES);
    //// Draw cross
    //glVertex2f(left, top);
    //glVertex2f(left, bottom);

    //glVertex2f(right, bottom);
    //glVertex2f(right, top);

    //glVertex2f(left, top);
    //glVertex2f(right, top);

    //glVertex2f(left, bottom);
    //glVertex2f(right, bottom);
    //glEnd();

    //glColor3f(1.0f, 1.0f, 1.0f);
    //glEnable(GL_TEXTURE_2D);
    //glEnable(GL_BLEND);
}

void HulkUpdate(Object* obj, uint32_t milliseconds)
{
    assert(obj != NULL);
    Hulk* Hulk_Obj = (Hulk*)obj;
    assert(Hulk_Obj != NULL);

    if (obj->IsActive == false || obj->IsPaused == true) return;

    // Calculate bounds every frame
    obj->objBounds = CalculateHulkBounds(obj->position);

    objSetRandomColor(obj, milliseconds);

    float rightSide = Hulk_Obj->Bounds.botRight.x;
    float leftSide = Hulk_Obj->Bounds.topLeft.x;
    float topSide = Hulk_Obj->Bounds.topLeft.y;
    float bottomSide = Hulk_Obj->Bounds.botRight.y;

    float scaledWidth = HULK_BOUND_X * HULK_SPRITE_SCALE;
    float scaledHeight = HULK_BOUND_Y * HULK_SPRITE_SCALE;

    // Check walking elapsed time
    Hulk_Obj->WalkElapsedTime += milliseconds;
    if (Hulk_Obj->WalkElapsedTime >= Hulk_Obj->WalkDuration)
    {
        Hulk_Obj->WalkElapsedTime = 0.0f;

        // Get random direction
        enum Direction HulkDir = (enum Direction)(rand() % 4);

        // Get random walk time
        float randWalkDuration = randGetFloat(HULK_MIN_WALK_DURATION, HULK_MAX_WALK_DURATION);

        Hulk_Obj->Obj.direction = HulkDir;
        Hulk_Obj->WalkDuration = randWalkDuration;
    }
    else
    {
        switch (Hulk_Obj->Obj.direction)
        {
        case Up: 
            // Check if movement is possible
            if (obj->position.y - scaledHeight / 2 < topSide)
            {
                Hulk_Obj->WalkElapsedTime = Hulk_Obj->WalkDuration;
                break;
            }
            Hulk_Obj->Obj.position.y -= Hulk_Obj->Obj.velocity.y * milliseconds; 
            PlayAnimation(Hulk_Obj->RunDownAnim, milliseconds);
            break;
        case Down: 
            // Check if movement is possible
            if (obj->position.y + scaledHeight / 2 > bottomSide)
            {
                Hulk_Obj->WalkElapsedTime = Hulk_Obj->WalkDuration;
                break;
            }
            Hulk_Obj->Obj.position.y += Hulk_Obj->Obj.velocity.y * milliseconds; 
            PlayAnimation(Hulk_Obj->RunDownAnim, milliseconds);
            break;
        case Left:
            // Check if movement is possible
            if (obj->position.x - scaledWidth / 2 < leftSide)
            {
                Hulk_Obj->WalkElapsedTime = Hulk_Obj->WalkDuration;
                break;
            }
            Hulk_Obj->Obj.position.x -= Hulk_Obj->Obj.velocity.x * milliseconds;
            PlayAnimation(Hulk_Obj->RunLeftAnim, milliseconds);
            break;
        case Right:
            // Check if movement is possible
            if (obj->position.x + scaledWidth / 2 > rightSide)
            {
                Hulk_Obj->WalkElapsedTime = Hulk_Obj->WalkDuration;
                break;
            }
            Hulk_Obj->Obj.position.x += Hulk_Obj->Obj.velocity.x * milliseconds;
            PlayAnimation(Hulk_Obj->RunRightAnim, milliseconds);
            break;
        }
    }
}

void HulkColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType)
{
    
}