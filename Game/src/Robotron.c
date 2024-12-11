#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <gl/GLU.h>
#include "SOIL.h"

#include "Robotron.h"
#include "random.h"
#include "input.h"
#include "BulletManager.h"
#include "stdbool.h"

static const char RobotronSprite[] = "asset/Player_White.png";

static GLuint RobotronTexture = 0;
static BulletManager* Manager = NULL;
static RobtronCollideCB _robotronCollideCB[2] = { NULL };
static bool IsCollidingWithStoppable = false;
static bool LostLife = false;

static void RobotronUpdate(Object* obj, uint32_t milliseconds);
static void RobotronDraw(Object* obj);
static void DisableBullet(Object* Obj);
static void RobotronColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType);
static void _robotronTriggerCollideCB(Robotron* Robotron_Obj, bool IsAlive);

static ObjVtable RobotronVtable = {
	RobotronDraw,
	RobotronUpdate
};

static ObjCollisionVtable RobotronCollVTable = {
    RobotronColl
};

void RobotronSetCollideCB(RobtronCollideCB cb, bool IsDead)
{
    if(IsDead == false)
        _robotronCollideCB[0] = cb;
    else
        _robotronCollideCB[1] = cb;
}

void RobotronClearCollideCB(bool IsDead)
{
    if(IsDead == false)
        _robotronCollideCB[0] = NULL;
    else
        _robotronCollideCB[1] = NULL;
}

/// @brief one time initialization of textures
void RobotronInitTextures()
{
	if (RobotronTexture == 0)
	{
        /*RobotronTexture = SOIL_load_OGL_texture(RobotronSprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		assert(RobotronTexture != 0);*/
        RobotronTexture = SOIL_load_OGL_texture(RobotronSprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
            SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB);
        assert(RobotronTexture != 0);
	}
}

void RobotronInitAnimations(Robotron* Robotron_Obj)
{
    assert(Robotron_Obj != NULL);

    // Running up animations
    SpriteFrame* RunUpSpriteFrames = malloc(sizeof(SpriteFrame) * RUNANIM_TYPES);
    assert(RunUpSpriteFrames != NULL);

    RunUpSpriteFrames[0].Position = (Coord2D){ RU_1_POS_X, RUN_POS_Y };
    RunUpSpriteFrames[0].Size = (Coord2D){ RU_SIZE_X , RU_SIZE_Y };

    RunUpSpriteFrames[1].Position = (Coord2D){ RU_2_POS_X, RUN_POS_Y };
    RunUpSpriteFrames[1].Size = (Coord2D){ RU_SIZE_X , RU_SIZE_Y };

    RunUpSpriteFrames[2].Position = (Coord2D){ RU_3_POS_X, RUN_POS_Y };
    RunUpSpriteFrames[2].Size = (Coord2D){ RU_SIZE_X , RU_SIZE_Y };

    Robotron_Obj->RunUpAnim = InitAnimation(RunUpSpriteFrames, RUNANIM_TYPES, 0, FRAME_DUR);

    // Running down animations
    SpriteFrame* RunDownSpriteFrames = malloc(sizeof(SpriteFrame) * RUNANIM_TYPES);
    assert(RunDownSpriteFrames != NULL);

    RunDownSpriteFrames[0].Position = (Coord2D){ RD_1_POS_X, RUN_POS_Y };
    RunDownSpriteFrames[0].Size = (Coord2D){ RD_SIZE_X , RD_SIZE_Y };

    RunDownSpriteFrames[1].Position = (Coord2D){ RD_2_POS_X, RUN_POS_Y };
    RunDownSpriteFrames[1].Size = (Coord2D){ RD_SIZE_X , RD_SIZE_Y };

    RunDownSpriteFrames[2].Position = (Coord2D){ RD_3_POS_X, RUN_POS_Y };
    RunDownSpriteFrames[2].Size = (Coord2D){ RD_SIZE_X , RD_SIZE_Y };

    Robotron_Obj->RunDownAnim = InitAnimation(RunDownSpriteFrames, RUNANIM_TYPES, 0, FRAME_DUR);

    // Running left animations
    SpriteFrame* RunLeftSpriteFrames = malloc(sizeof(SpriteFrame) * RUNANIM_TYPES);
    assert(RunLeftSpriteFrames != NULL);

    RunLeftSpriteFrames[0].Position = (Coord2D){ RL_1_POS_X, RUN_POS_Y };
    RunLeftSpriteFrames[0].Size = (Coord2D){ RL_SIZE_X , RL_SIZE_Y };

    RunLeftSpriteFrames[1].Position = (Coord2D){ RL_2_POS_X, RUN_POS_Y };
    RunLeftSpriteFrames[1].Size = (Coord2D){ RL_SIZE_X , RL_SIZE_Y };

    RunLeftSpriteFrames[2].Position = (Coord2D){ RL_3_POS_X, RUN_POS_Y };
    RunLeftSpriteFrames[2].Size = (Coord2D){ RL_SIZE_X , RL_SIZE_Y };

    Robotron_Obj->RunLeftAnim = InitAnimation(RunLeftSpriteFrames, RUNANIM_TYPES, 0, FRAME_DUR);

    // Running right animations
    SpriteFrame* RunRightSpriteFrames = malloc(sizeof(SpriteFrame) * RUNANIM_TYPES);
    assert(RunRightSpriteFrames != NULL);

    RunRightSpriteFrames[0].Position = (Coord2D){ RR_1_POS_X, RUN_POS_Y };
    RunRightSpriteFrames[0].Size = (Coord2D){ RR_SIZE_X , RR_SIZE_Y };

    RunRightSpriteFrames[1].Position = (Coord2D){ RR_2_POS_X, RUN_POS_Y };
    RunRightSpriteFrames[1].Size = (Coord2D){ RR_SIZE_X , RR_SIZE_Y };

    RunRightSpriteFrames[2].Position = (Coord2D){ RR_3_POS_X, RUN_POS_Y };
    RunRightSpriteFrames[2].Size = (Coord2D){ RR_SIZE_X , RR_SIZE_Y };

    Robotron_Obj->RunRightAnim = InitAnimation(RunRightSpriteFrames, RUNANIM_TYPES, 0,  FRAME_DUR);
}

Bounds2D CalculateRobotronBounds(Coord2D Position)
{
    float scaledWidth = BOUND_X * SPRITE_SCALE;
    float scaledHeight = BOUND_Y * SPRITE_SCALE;

    // calculate the bounding box
    float xPositionLeft = (Position.x - scaledWidth / 2);
    float xPositionRight = (Position.x + scaledWidth / 2);
    float yPositionTop = (Position.y - scaledHeight / 2);
    float yPositionBottom = (Position.y + scaledHeight / 2);

    return (Bounds2D) { { xPositionLeft, yPositionTop }, { xPositionRight, yPositionBottom } };
}

Robotron* RobotronNew(Coord2D Position, Bounds2D FieldBounds)
{
    Robotron* Robotron_Obj = malloc(sizeof(Robotron));
    if (Robotron_Obj != NULL)
    {
        Coord2D Vel = { DEF_VELOCITY, DEF_VELOCITY };
        objInit(&Robotron_Obj->Obj, &RobotronVtable, Position, Vel, CalculateRobotronBounds(Position), DefaultDir, DEFAULT_COLOR, DEFAULT_COLOR_CHANGE_DELAY, Character, &RobotronCollVTable);

        // Update the object color change delay for Robotron
        Robotron_Obj->Obj.colorChangeDelay = 400;

        RobotronInitAnimations(Robotron_Obj);

        Robotron_Obj->Bounds = FieldBounds;

        // Set the Robotron character sprite sheet size
        Robotron_Obj->SheetSize = (Coord2D){ 149.0f, 13.0f };

        Robotron_Obj->NumLives = DEF_LIVES;
        Robotron_Obj->MaxLives = MAX_LIVES;

        // Initializing bullet manager
        Manager = BulletManagerInit(FieldBounds);

        BulletSetCollideCB(DisableBullet, DefaultObjType);

    }

    return Robotron_Obj;
}

void RobotronDelete(Robotron* Robotron_Obj)
{
    assert(Robotron_Obj != NULL && Manager != NULL);

    BulletManagerDestroy(Manager);
    objDeinit(&Robotron_Obj->Obj);

    free(Robotron_Obj);
}

void RobotronDraw(Object* obj)
{
    assert(obj != NULL);
    Robotron* Robotron_Obj = (Robotron*)obj;
    assert(Robotron_Obj != NULL);

    // Choose the correct animation based on direction
    SpriteAnimation* CurrentAnimation;
    switch (obj->direction)
    {
    case Up: CurrentAnimation = Robotron_Obj->RunUpAnim; break;
    case Down: CurrentAnimation = Robotron_Obj->RunDownAnim; break;
    case Left: CurrentAnimation = Robotron_Obj->RunLeftAnim; break;
    case Right: CurrentAnimation = Robotron_Obj->RunRightAnim; break;
    case UpLeft:
    case DownLeft: CurrentAnimation = Robotron_Obj->RunLeftAnim; break;
    case UpRight:
    case DownRight: CurrentAnimation = Robotron_Obj->RunRightAnim; break;
    default: CurrentAnimation = Robotron_Obj->RunDownAnim; break; // default to Down if none
    }

    // Get the current frame of the selected animation
    SpriteFrame* CurrentFrame = GetCurrentFrame(CurrentAnimation);

    assert(CurrentFrame != NULL);

    // Enable blending to apply color tinting
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, RobotronTexture);
    // Disable anti-aliasing
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBegin(GL_TRIANGLE_STRIP);
    {
        // Scale the sprite
        GLfloat scaledWidth = CurrentFrame->Size.x * SPRITE_SCALE;
        GLfloat scaledHeight = CurrentFrame->Size.y * SPRITE_SCALE;

        // calculate the bounding box
        GLfloat xPositionLeft = (obj->position.x - scaledWidth / 2);
        GLfloat xPositionRight = (obj->position.x + scaledWidth / 2);
        GLfloat yPositionTop = (obj->position.y - scaledHeight / 2);
        GLfloat yPositionBottom = (obj->position.y + scaledHeight / 2);

        // Calculate the UV coordinates for the sprite based on its position in the sprite sheet
        GLfloat xTextureCoord = CurrentFrame->Position.x / Robotron_Obj->SheetSize.x;
        GLfloat yTextureCoord = 1.0f - (CurrentFrame->Position.y / Robotron_Obj->SheetSize.y);
        GLfloat uWidth = CurrentFrame->Size.x / Robotron_Obj->SheetSize.x;
        GLfloat vHeight = CurrentFrame->Size.y / Robotron_Obj->SheetSize.y;

        const float BG_DEPTH = -0.99f;

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

        if(LostLife)
            glColor4ub(r, g, b, 0xFF);
        else
            glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);

        // BL
        glTexCoord2f(xTextureCoord, yTextureCoord - vHeight);
        glVertex3f(xPositionLeft, yPositionBottom, BG_DEPTH);

        glColor4ub(r, g, b, 0xFF);

        // TR
        glTexCoord2f(xTextureCoord + uWidth, yTextureCoord);
        glVertex3f(xPositionRight, yPositionTop, BG_DEPTH);

        if (LostLife)
            glColor4ub(r, g, b, 0xFF);
        else
            glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);

        // BR
        glTexCoord2f(xTextureCoord + uWidth, yTextureCoord - vHeight);
        glVertex3f(xPositionRight, yPositionBottom, BG_DEPTH);

    }
    glEnd();
}

void RobotronSetDeathColor(Object* obj, uint32_t milliseconds)
{
    assert(obj != NULL);

    obj->colorUpdateElapsedTime += milliseconds;

    if (obj->colorUpdateElapsedTime < obj->colorChangeDelay)
        return;
    else
        obj->colorUpdateElapsedTime = 0;

    obj->color = randGetInt(0, 60);
    obj->color += randGetInt(0, 60) << 8;
    obj->color += randGetInt(0, 60) << 16;
}

void RobotronUpdate(Object* obj, uint32_t milliseconds)
{
    if (obj->IsPaused == true)
    {
        LostLife = true;
        RobotronSetDeathColor(obj, milliseconds);
        //objSetRandomColor(obj, milliseconds);
    }
    else
    {
        LostLife = false;
        objSetRandomColor(obj, milliseconds);
        HandleMovementInput(obj, milliseconds);
        HandleAimingInput(obj, milliseconds);

        // Calculate bounds every frame
        obj->objBounds = CalculateRobotronBounds(obj->position);
    }
}

void HandleMovementInput(Object* obj, uint32_t milliseconds)
{
    assert(obj != NULL);
    Robotron* Robotron_Obj = (Robotron*)obj;
    assert(Robotron_Obj != NULL);

    SpriteAnimation* currentAnimation = NULL;

    float rightSide = Robotron_Obj->Bounds.botRight.x;
    float leftSide = Robotron_Obj->Bounds.topLeft.x;
    float topSide = Robotron_Obj->Bounds.topLeft.y;
    float bottomSide = Robotron_Obj->Bounds.botRight.y;

    float scaledWidth = BOUND_X * SPRITE_SCALE;
    float scaledHeight = BOUND_Y * SPRITE_SCALE;

    if (inputKeyPressed(0x57) && inputKeyPressed(0x41)) // W and A keys
    {
        // Check if movement is possible
        if (obj->position.x - scaledWidth / 2 < leftSide || obj->position.y - scaledHeight / 2 < topSide) return;

        ResetAnimation(Robotron_Obj->RunUpAnim);
        ResetAnimation(Robotron_Obj->RunDownAnim);
        ResetAnimation(Robotron_Obj->RunRightAnim);
        // Move up and left
        obj->direction = UpLeft;
        obj->position.x -= (obj->velocity.x * CHAR_DIAGONAL_SPEED * milliseconds);
        obj->position.y -= (obj->velocity.y * CHAR_DIAGONAL_SPEED * milliseconds);
        currentAnimation = Robotron_Obj->RunLeftAnim;
    }

    else if (inputKeyPressed(0x57) && inputKeyPressed(0x44)) // W and D keys
    {
        // Check if movement is possible
        if (obj->position.x + scaledWidth / 2 > rightSide || obj->position.y - scaledHeight / 2 < topSide) return;

        ResetAnimation(Robotron_Obj->RunUpAnim);
        ResetAnimation(Robotron_Obj->RunDownAnim);
        ResetAnimation(Robotron_Obj->RunLeftAnim);
        // Move up and right
        obj->direction = UpRight;
        obj->position.x += (obj->velocity.x * CHAR_DIAGONAL_SPEED * milliseconds);
        obj->position.y -= (obj->velocity.y * CHAR_DIAGONAL_SPEED * milliseconds);
        currentAnimation = Robotron_Obj->RunRightAnim;
    }

    else if (inputKeyPressed(0x53) && inputKeyPressed(0x41)) // S and A keys
    {
        // Check if movement is possible
        if (obj->position.x - scaledWidth / 2 < leftSide || obj->position.y + scaledHeight / 2 > bottomSide) return;

        ResetAnimation(Robotron_Obj->RunUpAnim);
        ResetAnimation(Robotron_Obj->RunDownAnim);
        ResetAnimation(Robotron_Obj->RunRightAnim);
        // Move down and left
        obj->direction = DownLeft;
        obj->position.x -= (obj->velocity.x * CHAR_DIAGONAL_SPEED * milliseconds);
        obj->position.y += (obj->velocity.y * CHAR_DIAGONAL_SPEED * milliseconds);
        currentAnimation = Robotron_Obj->RunLeftAnim;
    }

    else if (inputKeyPressed(0x53) && inputKeyPressed(0x44)) // S and D keys
    {
        // Check if movement is possible
        if (obj->position.x + scaledWidth / 2 > rightSide || obj->position.y + scaledHeight / 2 > bottomSide) return;

        ResetAnimation(Robotron_Obj->RunUpAnim);
        ResetAnimation(Robotron_Obj->RunDownAnim);
        ResetAnimation(Robotron_Obj->RunLeftAnim);
        // Move down and right
        obj->direction = DownRight;
        obj->position.x += (obj->velocity.x * CHAR_DIAGONAL_SPEED * milliseconds);
        obj->position.y += (obj->velocity.y * CHAR_DIAGONAL_SPEED * milliseconds);
        currentAnimation = Robotron_Obj->RunRightAnim;
    }

    else if (inputKeyPressed(0x57)) // W key
    {
        // Check if movement is possible
        if (obj->position.y - scaledHeight / 2 < topSide) return;

        ResetAnimation(Robotron_Obj->RunDownAnim);
        ResetAnimation(Robotron_Obj->RunLeftAnim);
        ResetAnimation(Robotron_Obj->RunRightAnim);
        // Move up
        obj->direction = Up;
        obj->position.y -= (obj->velocity.y * milliseconds);
        currentAnimation = Robotron_Obj->RunUpAnim;
    }

    else if (inputKeyPressed(0x41)) // A key
    {
        // Check if movement is possible
        if (obj->position.x - scaledWidth / 2 < leftSide) return;

        ResetAnimation(Robotron_Obj->RunUpAnim);
        ResetAnimation(Robotron_Obj->RunDownAnim);
        ResetAnimation(Robotron_Obj->RunRightAnim);
        // Move left
        obj->direction = Left;
        obj->position.x -= (obj->velocity.x * milliseconds);
        currentAnimation = Robotron_Obj->RunLeftAnim;
    }

    else if (inputKeyPressed(0x53)) // S key
    {
        // Check if movement is possible
        if (obj->position.y + scaledHeight / 2 > bottomSide) return;

        ResetAnimation(Robotron_Obj->RunUpAnim);
        ResetAnimation(Robotron_Obj->RunLeftAnim);
        ResetAnimation(Robotron_Obj->RunRightAnim);
        // Move down
        obj->direction = Down;
        obj->position.y += (obj->velocity.y * milliseconds);
        currentAnimation = Robotron_Obj->RunDownAnim;
    }

    else if (inputKeyPressed(0x44)) // D key
    {
        // Check if movement is possible
        if (obj->position.x + scaledWidth / 2 > rightSide) return;

        ResetAnimation(Robotron_Obj->RunUpAnim);
        ResetAnimation(Robotron_Obj->RunDownAnim);
        ResetAnimation(Robotron_Obj->RunLeftAnim);
        // Move right
        obj->direction = Right;
        obj->position.x += (obj->velocity.x * milliseconds);
        currentAnimation = Robotron_Obj->RunRightAnim;
    }

    // Only play the animation if a direction key is pressed
    if (currentAnimation != NULL)
    {
        PlayAnimation(currentAnimation, milliseconds);
    }
    else
    {
        ResetAnimation(Robotron_Obj->RunUpAnim);
        ResetAnimation(Robotron_Obj->RunDownAnim);
        ResetAnimation(Robotron_Obj->RunLeftAnim);
        ResetAnimation(Robotron_Obj->RunRightAnim);
    }
}

void HandleAimingInput(Object* obj, uint32_t milliseconds)
{
    // Using I,J,K,L for Aiming

    assert(obj != NULL);
    Robotron* Robotron_Obj = (Robotron*)obj;
    assert(Robotron_Obj != NULL);

    bool ShootKeyPressed = false;

    if (inputKeyPressed(0x49) && inputKeyPressed(0x4A))
    {
        AimDirection = UpLeft;
        ShootKeyPressed = true;
    }
    else if (inputKeyPressed(0x49) && inputKeyPressed(0x4C))
    {
        AimDirection = UpRight;
        ShootKeyPressed = true;
    }
    else if (inputKeyPressed(0x4B) && inputKeyPressed(0x4A))
    {
        AimDirection = DownLeft;
        ShootKeyPressed = true;
    }
    else if (inputKeyPressed(0x4B) && inputKeyPressed(0x4C))
    {
        AimDirection = DownRight;
        ShootKeyPressed = true;
    }
    else if (inputKeyPressed(0x49))
    {
        AimDirection = Up;
        ShootKeyPressed = true;
    }
    else if (inputKeyPressed(0x4B))
    {
        AimDirection = Down;
        ShootKeyPressed = true;
    }
    else if (inputKeyPressed(0x4A))
    {
        AimDirection = Left;
        ShootKeyPressed = true;
    }
    else if (inputKeyPressed(0x4C))
    {
        AimDirection = Right;
        ShootKeyPressed = true;
    }

    if (ShootKeyPressed == true)
    {
        BulletManagerSpawnBullet(Manager, Robotron_Obj->Obj.position, AimDirection, milliseconds);
    }
}

void RobotronRecycleAllBullets()
{
    BulletManagerRecycleAllBullets(Manager);
}

static void DisableBullet(Object* Obj)
{
    assert(Obj != NULL);
    if (Obj->objtype != Projectile) return;

    Bullet* Bullet_Obj = (Bullet*)Obj;

    BulletManagerRecycleBullet(Manager, Bullet_Obj);
}

void RobotronColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType)
{
    assert(SourceObject != NULL && TargetObject != NULL);
    Robotron* Robotron_Obj = (Robotron*)SourceObject;
    assert(Robotron_Obj != NULL);

    if (SourceObject->IsActive == false || TargetObject->IsActive == false || SourceObject->IsPaused || TargetObject->IsPaused ||
        SourceObject->CanSpawn == false || TargetObject->CanSpawn == false) return;

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
                         SrcBottomSide < TargetTopSide || // Source is above Target
                         SrcTopSide > TargetBottomSide);  // Source is below Target

    if (isColliding == true)
    {
        if (TargetObjectType == EnemyGrunt || TargetObjectType == EnemyHulk || TargetObjectType == EnemyEnforcer || TargetObjectType == Obstacle || TargetObjectType == EnemyTank)
        {
            ReduceLives(Robotron_Obj);
        }
        /*else if (TargetObjectType == EnemyHulk)
        {
            char message[63];
            sprintf_s(message, sizeof(message), "IsCollidingWithStoppable = %d\n", IsCollidingWithStoppable);
            OutputDebugStringA(message);
            SourceObject->velocity = (Coord2D){ 0.0f,0.0f };
            IsCollidingWithStoppable = true;
        }*/
    }
    /*else if(IsCollidingWithStoppable)
    {
        SourceObject->velocity = (Coord2D){ DEF_VELOCITY, DEF_VELOCITY };
        IsCollidingWithStoppable = false;
    }*/
}

uint8_t GetCurrentLives(Robotron* Robotron_Obj)
{
    return Robotron_Obj->NumLives;
}

void ResetLives(Robotron* Robotron_Obj)
{
    Robotron_Obj->NumLives = DEF_LIVES;
}

void ReduceLives(Robotron* Robotron_Obj)
{
    if (Robotron_Obj->NumLives - 1 == 0)
    {
        --Robotron_Obj->NumLives;

        // Recycle all active bullets
        BulletManagerRecycleAllBullets(Manager);

        // Game Over State
        _robotronTriggerCollideCB(Robotron_Obj, true);
    }
    else
    {
        --Robotron_Obj->NumLives;

        // Recycle all active bullets
        BulletManagerRecycleAllBullets(Manager);

        // State Reset
        _robotronTriggerCollideCB(Robotron_Obj, false);
    }
}

void IncreaseLives(Robotron* Robotron_Obj)
{
    ++Robotron_Obj->NumLives;
    if (Robotron_Obj->NumLives > Robotron_Obj->MaxLives)
        Robotron_Obj->NumLives = Robotron_Obj->MaxLives;
}

void _robotronTriggerCollideCB(Robotron* Robotron_Obj, bool IsAlive)
{
    if (IsAlive == false && _robotronCollideCB[0] != NULL)
        _robotronCollideCB[0](Robotron_Obj);
    else if(IsAlive == true && _robotronCollideCB[1] != NULL)
        _robotronCollideCB[1](Robotron_Obj);
}