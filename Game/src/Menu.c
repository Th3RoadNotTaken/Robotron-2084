#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <gl/GLU.h>
#include "SOIL.h"

#include "Menu.h"
#include "random.h"
#include "stdbool.h"

static const char MainMenuSprite[] = "asset/MainMenu.png";
static const char StartButtonSprite[] = "asset/MainMenuStart.png";
static const char GameOverSprite[] = "asset/GameOver.png";

static GLuint MainMenuTexture = 0;
static GLuint StartButtonTexture = 0;
static GLuint GameOverTexture = 0;

static void MenuUpdate(Object* obj, uint32_t milliseconds);
static void MenuDraw(Object* obj);
static void MenuColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType);

static ObjVtable MenuVtable = {
    MenuDraw,
    MenuUpdate
};
static ObjCollisionVtable MenuCollVTable = {
    MenuColl
};

void MenuInitTextures()
{
    if (MainMenuTexture == 0)
    {
        MainMenuTexture = SOIL_load_OGL_texture(MainMenuSprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
            SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB);
        assert(MainMenuTexture != 0);
    }
    if (StartButtonTexture == 0)
    {
        StartButtonTexture = SOIL_load_OGL_texture(StartButtonSprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
            SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB);
        assert(StartButtonTexture != 0);
    }
    if (GameOverTexture == 0)
    {
        GameOverTexture = SOIL_load_OGL_texture(GameOverSprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
            SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB);
        assert(GameOverTexture != 0);
    }
}

void MenuInitFrames(Menu* Menu_Obj)
{
    assert(Menu_Obj != NULL);

    // Initialize main menu sprite frame
    Menu_Obj->MenuSpriteFrame.Position = (Coord2D){ MM_POS_X, MM_POS_Y };
    Menu_Obj->MenuSpriteFrame.Size = (Coord2D){ MM_SIZE_X , MM_SIZE_Y };

    // Initialize start button sprite frame
    Menu_Obj->StartSpriteFrame.Position = (Coord2D){ SB_POS_X, SB_POS_Y };
    Menu_Obj->StartSpriteFrame.Size = (Coord2D){ SB_SIZE_X , SB_SIZE_Y };

    // Initialize game over sprite frame
    Menu_Obj->GameOverSpriteFrame.Position = (Coord2D){ GO_POS_X, GO_POS_Y };
    Menu_Obj->GameOverSpriteFrame.Size = (Coord2D){ GO_SIZE_X , GO_SIZE_Y };
}

Menu* MenuNew(Coord2D MainMenuPos, Coord2D StartPos, Coord2D GameOverPos, uint8_t LevelNumber)
{
    Menu* Menu_Obj = malloc(sizeof(Menu));
    if (Menu_Obj != NULL)
    {
        objInitDefault(&Menu_Obj->Obj, &MenuVtable, (Coord2D) { 0, 0 }, (Coord2D) { 0, 0 }, UI, & MenuCollVTable);

        Menu_Obj->Obj.colorChangeDelay = 200;

        MenuInitFrames(Menu_Obj);

        Menu_Obj->MenuPos = MainMenuPos;

        Menu_Obj->StartPos = StartPos;

        Menu_Obj->GameOverPos = GameOverPos;

        Menu_Obj->LevelNumber = LevelNumber;

        // Set the main menu sprite sheet size
        Menu_Obj->MenuSheetSize = (Coord2D){ 292.0f, 240.0f };

        // Set the start button sprite sheet size
        Menu_Obj->StartSheetSize = (Coord2D){ 77.0f, 16.0f };

        // Set the game over sprite sheet size
        Menu_Obj->GameOverSheetSize = (Coord2D){ 100.0f, 100.0f };
    }

    return Menu_Obj;
}

void MenuDelete(Menu* Menu_Obj)
{
    assert(Menu_Obj != NULL);

    objDeinit(&Menu_Obj->Obj);

    free(Menu_Obj);
}

void MenuDraw(Object* obj)
{
    assert(obj != NULL);
    Menu* Menu_Obj = (Menu*)obj;
    assert(Menu_Obj != NULL);

    if (Menu_Obj->LevelNumber == 0)
    {
        /* Main menu */
        // Scale the menu sprite
        GLfloat menuScaledWidth = (GLfloat)(Menu_Obj->MenuSpriteFrame.Size.x * MM_SCALE);
        GLfloat menuScaledHeight = (GLfloat)(Menu_Obj->MenuSpriteFrame.Size.y * MM_SCALE);

        // Draw the menu
        Coord2D MenuStartPos = Menu_Obj->MenuPos;

        // Calculate the bounding box
        GLfloat xPositionLeft = (MenuStartPos.x - menuScaledWidth / 2);
        GLfloat xPositionRight = (MenuStartPos.x + menuScaledWidth / 2);
        GLfloat yPositionTop = (MenuStartPos.y - menuScaledHeight / 2);
        GLfloat yPositionBottom = (MenuStartPos.y + menuScaledHeight / 2);

        // Calculate the UV coordinates for the sprite based on its position in the sprite sheet
        GLfloat xTextureCoord = Menu_Obj->MenuSpriteFrame.Position.x / Menu_Obj->MenuSheetSize.x;
        GLfloat yTextureCoord = 1.0f - (Menu_Obj->MenuSpriteFrame.Position.y / Menu_Obj->MenuSheetSize.y);
        GLfloat uWidth = Menu_Obj->MenuSpriteFrame.Size.x / Menu_Obj->MenuSheetSize.x;
        GLfloat vHeight = Menu_Obj->MenuSpriteFrame.Size.y / Menu_Obj->MenuSheetSize.y;

        const float BG_DEPTH = -0.99f;

        // Enable blending to apply color tinting
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, MainMenuTexture);
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


        /* Start button */
        // Scale the start button sprite
        GLfloat startScaledWidth = (GLfloat)(Menu_Obj->StartSpriteFrame.Size.x * SB_SCALE);
        GLfloat startScaledHeight = (GLfloat)(Menu_Obj->StartSpriteFrame.Size.y * SB_SCALE);

        // Draw the start button
        Coord2D StartButtonStartPos = Menu_Obj->StartPos;

        // Calculate the bounding box
        xPositionLeft = (StartButtonStartPos.x - startScaledWidth / 2);
        xPositionRight = (StartButtonStartPos.x + startScaledWidth / 2);
        yPositionTop = (StartButtonStartPos.y - startScaledHeight / 2);
        yPositionBottom = (StartButtonStartPos.y + startScaledHeight / 2);

        // Calculate the UV coordinates for the sprite based on its position in the sprite sheet
        xTextureCoord = Menu_Obj->StartSpriteFrame.Position.x / Menu_Obj->StartSheetSize.x;
        yTextureCoord = 1.0f - (Menu_Obj->StartSpriteFrame.Position.y / Menu_Obj->StartSheetSize.y);
        uWidth = Menu_Obj->StartSpriteFrame.Size.x / Menu_Obj->StartSheetSize.x;
        vHeight = Menu_Obj->StartSpriteFrame.Size.y / Menu_Obj->StartSheetSize.y;

        const float START_BUTTON_DEPTH = -0.98f;

        // Enable blending to apply color tinting
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, StartButtonTexture);
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
            glVertex3f(xPositionLeft, yPositionTop, START_BUTTON_DEPTH);

            // BL
            glTexCoord2f(xTextureCoord, yTextureCoord - vHeight);
            glVertex3f(xPositionLeft, yPositionBottom, START_BUTTON_DEPTH);

            // TR
            glTexCoord2f(xTextureCoord + uWidth, yTextureCoord);
            glVertex3f(xPositionRight, yPositionTop, START_BUTTON_DEPTH);

            // BR
            glTexCoord2f(xTextureCoord + uWidth, yTextureCoord - vHeight);
            glVertex3f(xPositionRight, yPositionBottom, START_BUTTON_DEPTH);
        }
        glEnd();

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
    }
    else if (Menu_Obj->LevelNumber == 2)
    {
        /* Game over screen */
        // Scale the game over sprite
        GLfloat gameOverScaledWidth = (GLfloat)(Menu_Obj->GameOverSpriteFrame.Size.x * GO_SCALE_X);
        GLfloat gameOverScaledHeight = (GLfloat)(Menu_Obj->GameOverSpriteFrame.Size.y * GO_SCALE_Y);

        // Draw the game over screen
        Coord2D GOStartPos = Menu_Obj->GameOverPos;

        // Calculate the bounding box
        GLfloat xPositionLeft = (GOStartPos.x - gameOverScaledWidth / 2);
        GLfloat xPositionRight = (GOStartPos.x + gameOverScaledWidth / 2);
        GLfloat yPositionTop = (GOStartPos.y - gameOverScaledHeight / 2);
        GLfloat yPositionBottom = (GOStartPos.y + gameOverScaledHeight / 2);

        // Calculate the UV coordinates for the sprite based on its position in the sprite sheet
        GLfloat xTextureCoord = Menu_Obj->GameOverSpriteFrame.Position.x / Menu_Obj->GameOverSheetSize.x;
        GLfloat yTextureCoord = 1.0f - (Menu_Obj->GameOverSpriteFrame.Position.y / Menu_Obj->GameOverSheetSize.y);
        GLfloat uWidth = Menu_Obj->GameOverSpriteFrame.Size.x / Menu_Obj->GameOverSheetSize.x;
        GLfloat vHeight = Menu_Obj->GameOverSpriteFrame.Size.y / Menu_Obj->GameOverSheetSize.y;

        const float BG_DEPTH = -0.99f;

        // Enable blending to apply color tinting
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, GameOverTexture);
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

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
    }
}

void MenuUpdate(Object* obj, uint32_t milliseconds)
{
    assert(obj != NULL);

    objSetRandomColor(obj, milliseconds);
}

void MenuColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType)
{
}