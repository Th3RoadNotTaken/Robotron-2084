#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <gl/GLU.h>
#include "SOIL.h"

#include "UI.h"
#include "random.h"
#include "stdbool.h"

static const char ScoreSprite[] = "asset/Numbers.png";
static const char LivesSprite[] = "asset/Lives.png";
static const char WaveSprite[] = "asset/Wave.png";

static GLuint ScoreTexture = 0;
static GLuint LivesTexture = 0;
static GLuint WaveTexture = 0;

static void UIUpdate(Object* obj, uint32_t milliseconds);
static void UIDraw(Object* obj);
static void UIColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType);

static ObjVtable UIVtable = {
	UIDraw,
	UIUpdate
};
static ObjCollisionVtable UICollVTable = {
	UIColl
};

void UIInitTextures()
{
	if (ScoreTexture == 0)
	{
		ScoreTexture = SOIL_load_OGL_texture(ScoreSprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB);
		assert(ScoreTexture != 0);
	}
    if (WaveTexture == 0)
    {
        WaveTexture = SOIL_load_OGL_texture(WaveSprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
            SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB);
        assert(WaveTexture != 0);
    }
    if (LivesTexture == 0)
    {
        LivesTexture = SOIL_load_OGL_texture(LivesSprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
            SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB);
        assert(LivesTexture != 0);
    }
}

void ScoreInitFrames(UIElements* UI_Obj)
{
    assert(UI_Obj != NULL);

    // Initialize number sprite frames
	UI_Obj->NumberFrames[0].Position = (Coord2D){ SCORE_0_POS_X, SCORE_POS_Y };
	UI_Obj->NumberFrames[0].Size = (Coord2D){ SCORE_SIZE_X , SCORE_SIZE_Y };

	UI_Obj->NumberFrames[1].Position = (Coord2D){ SCORE_1_POS_X, SCORE_POS_Y };
	UI_Obj->NumberFrames[1].Size = (Coord2D){ SCORE_SIZE_X , SCORE_SIZE_Y };

	UI_Obj->NumberFrames[2].Position = (Coord2D){ SCORE_2_POS_X, SCORE_POS_Y };
	UI_Obj->NumberFrames[2].Size = (Coord2D){ SCORE_SIZE_X , SCORE_SIZE_Y };

	UI_Obj->NumberFrames[3].Position = (Coord2D){ SCORE_3_POS_X, SCORE_POS_Y };
	UI_Obj->NumberFrames[3].Size = (Coord2D){ SCORE_SIZE_X , SCORE_SIZE_Y };

	UI_Obj->NumberFrames[4].Position = (Coord2D){ SCORE_4_POS_X, SCORE_POS_Y };
	UI_Obj->NumberFrames[4].Size = (Coord2D){ SCORE_SIZE_X , SCORE_SIZE_Y };

	UI_Obj->NumberFrames[5].Position = (Coord2D){ SCORE_5_POS_X, SCORE_POS_Y };
	UI_Obj->NumberFrames[5].Size = (Coord2D){ SCORE_SIZE_X , SCORE_SIZE_Y };

	UI_Obj->NumberFrames[6].Position = (Coord2D){ SCORE_6_POS_X, SCORE_POS_Y };
	UI_Obj->NumberFrames[6].Size = (Coord2D){ SCORE_SIZE_X , SCORE_SIZE_Y };

	UI_Obj->NumberFrames[7].Position = (Coord2D){ SCORE_7_POS_X, SCORE_POS_Y };
	UI_Obj->NumberFrames[7].Size = (Coord2D){ SCORE_SIZE_X , SCORE_SIZE_Y };

	UI_Obj->NumberFrames[8].Position = (Coord2D){ SCORE_8_POS_X, SCORE_POS_Y };
	UI_Obj->NumberFrames[8].Size = (Coord2D){ SCORE_SIZE_X , SCORE_SIZE_Y };

	UI_Obj->NumberFrames[9].Position = (Coord2D){ SCORE_9_POS_X, SCORE_POS_Y };
	UI_Obj->NumberFrames[9].Size = (Coord2D){ SCORE_SIZE_X , SCORE_SIZE_Y };

    // Initialize lives sprite frame
    UI_Obj->LivesFrame.Position = (Coord2D){ LIVES_POS_X , LIVES_POS_Y };
    UI_Obj->LivesFrame.Size = (Coord2D){ LIVES_SIZE_X , LIVES_SIZE_Y };

    // Initialize wave text sprite frame
    UI_Obj->WaveTextFrame.Position = (Coord2D){ WAVE_POS_X , WAVE_POS_Y };
    UI_Obj->WaveTextFrame.Size = (Coord2D){ WAVE_SIZE_X , WAVE_SIZE_Y };
}

UIElements* UINew(Bounds2D FieldBounds, uint8_t LevelNumber, bool IsActive, Coord2D ScorePos, Coord2D HighScoresPos, uint8_t LivesVal, Coord2D LivesPos, Coord2D WaveTextPos, uint32_t WaveNum, Coord2D WaveNumPos)
{
	UIElements* UI_Obj = malloc(sizeof(UIElements));
	if (UI_Obj != NULL)
	{
		objInitDefault(&UI_Obj->Obj, &UIVtable, (Coord2D) { 0, 0 }, (Coord2D) { 0, 0 }, UI, &UICollVTable);

		UI_Obj->Obj.colorChangeDelay = 200;

		ScoreInitFrames(UI_Obj);

		UI_Obj->Bounds = FieldBounds;

		UI_Obj->Obj.IsActive = IsActive;

		UI_Obj->ScoreVal = 0;
		UI_Obj->ScorePos = ScorePos;
        UI_Obj->HighScoresPos = HighScoresPos;
        UI_Obj->ScoreSpriteScale = LevelNumber == 1 ? SCORE_SPRITE_SCALE : FINAL_SCORE_SPRITE_SCALE;
        UI_Obj->NumberGap = LevelNumber == 1 ? NUMBER_GAP : FINAL_SCORE_NUMBER_GAP;

        UI_Obj->LivesVal = LivesVal;
        UI_Obj->LivesPos = LivesPos;

        UI_Obj->WaveNum = WaveNum;
        UI_Obj->WaveTextPos = WaveTextPos;
        UI_Obj->WaveNumPos = WaveNumPos;

        UI_Obj->LevelNumber = LevelNumber;

		// Set the number sprite sheet size
		UI_Obj->NumberSheetSize = (Coord2D){ 84.0f, 5.0f };

        // Set the lives sprite sheet size
        UI_Obj->LivesSheetSize = (Coord2D){ 7.0f, 11.0f };

        // Set the wave text sprite sheet size
        UI_Obj->WaveTextSheetSize = (Coord2D){ 23.0f, 6.0f };
	}

	return UI_Obj;
}

void UIDelete(UIElements* UI_Obj)
{
	assert(UI_Obj != NULL);

	objDeinit(&UI_Obj->Obj);

	free(UI_Obj);
}

void UIDraw(Object* obj)
{
    assert(obj != NULL);
    UIElements* UI_Obj = (UIElements*)obj;
    assert(UI_Obj != NULL);

    if (obj->IsActive == false) return;

    /* Score Val */
    // Scale the number sprite
    GLfloat scaledWidth = (GLfloat)(UI_Obj->NumberFrames[0].Size.x * UI_Obj->ScoreSpriteScale);
    GLfloat scaledHeight = (GLfloat)(UI_Obj->NumberFrames[0].Size.y * UI_Obj->ScoreSpriteScale);

    // Draw the score
    uint32_t Score = UI_Obj->ScoreVal;
    Coord2D ScoreStartPos = UI_Obj->ScorePos;

    if (Score == 0)
    {
        // Calculate the bounding box
        GLfloat xPositionLeft = (ScoreStartPos.x - scaledWidth / 2);
        GLfloat xPositionRight = (ScoreStartPos.x + scaledWidth / 2);
        GLfloat yPositionTop = (ScoreStartPos.y - scaledHeight / 2);
        GLfloat yPositionBottom = (ScoreStartPos.y + scaledHeight / 2);

        // Calculate the UV coordinates for the sprite based on its position in the sprite sheet
        GLfloat xTextureCoord = UI_Obj->NumberFrames[0].Position.x / UI_Obj->NumberSheetSize.x;
        GLfloat yTextureCoord = 1.0f - (UI_Obj->NumberFrames[0].Position.y / UI_Obj->NumberSheetSize.y);
        GLfloat uWidth = UI_Obj->NumberFrames[0].Size.x / UI_Obj->NumberSheetSize.x;
        GLfloat vHeight = UI_Obj->NumberFrames[0].Size.y / UI_Obj->NumberSheetSize.y;

        const float BG_DEPTH = -0.98f;

        // Enable blending to apply color tinting
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, ScoreTexture);
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

        ScoreStartPos.x -= UI_Obj->NumberGap;
    }

    while (Score > 0)
    {
        uint8_t Digit = Score % 10;

        // Draw the digit
 
        // Calculate the bounding box
        GLfloat xPositionLeft = (ScoreStartPos.x - scaledWidth / 2);
        GLfloat xPositionRight = (ScoreStartPos.x + scaledWidth / 2);
        GLfloat yPositionTop = (ScoreStartPos.y - scaledHeight / 2);
        GLfloat yPositionBottom = (ScoreStartPos.y + scaledHeight / 2);

        // Calculate the UV coordinates for the sprite based on its position in the sprite sheet
        GLfloat xTextureCoord = UI_Obj->NumberFrames[Digit].Position.x / UI_Obj->NumberSheetSize.x;
        GLfloat yTextureCoord = 1.0f - (UI_Obj->NumberFrames[Digit].Position.y / UI_Obj->NumberSheetSize.y);
        GLfloat uWidth = UI_Obj->NumberFrames[Digit].Size.x / UI_Obj->NumberSheetSize.x;
        GLfloat vHeight = UI_Obj->NumberFrames[Digit].Size.y / UI_Obj->NumberSheetSize.y;

        const float BG_DEPTH = -0.98f;

        // Enable blending to apply color tinting
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, ScoreTexture);
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

        ScoreStartPos.x -= UI_Obj->NumberGap;
        Score /= 10;
    }

    if (UI_Obj->LevelNumber != 2)
    {
        /* Lives Val */
        // Scale the lives sprite
        GLfloat livesScaledWidth = (GLfloat)(UI_Obj->LivesFrame.Size.x * LIVES_SPRITE_SCALE);
        GLfloat livesScaledHeight = (GLfloat)(UI_Obj->LivesFrame.Size.y * LIVES_SPRITE_SCALE);

        // Draw the lives
        uint8_t Lives = UI_Obj->LivesVal;
        Coord2D LivesStartPos = UI_Obj->LivesPos;

        while (Lives > 0)
        {
            // Draw each life sprite

            // Calculate the bounding box
            GLfloat xPositionLeft = (LivesStartPos.x - livesScaledWidth / 2);
            GLfloat xPositionRight = (LivesStartPos.x + livesScaledWidth / 2);
            GLfloat yPositionTop = (LivesStartPos.y - livesScaledHeight / 2);
            GLfloat yPositionBottom = (LivesStartPos.y + livesScaledHeight / 2);

            // Calculate the UV coordinates for the sprite based on its position in the sprite sheet
            GLfloat xTextureCoord = UI_Obj->LivesFrame.Position.x / UI_Obj->LivesSheetSize.x;
            GLfloat yTextureCoord = 1.0f - (UI_Obj->LivesFrame.Position.y / UI_Obj->LivesSheetSize.y);
            GLfloat uWidth = UI_Obj->LivesFrame.Size.x / UI_Obj->LivesSheetSize.x;
            GLfloat vHeight = UI_Obj->LivesFrame.Size.y / UI_Obj->LivesSheetSize.y;

            const float BG_DEPTH = -0.99f;

            // Enable blending to apply color tinting
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);

            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, LivesTexture);
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

            glDisable(GL_TEXTURE_2D);
            glDisable(GL_BLEND);

            --Lives;
            LivesStartPos.x += LIVES_GAP;
        }

        /* Wave text */
        // Scale the text sprite
        GLfloat waveScaledWidth = (GLfloat)(UI_Obj->WaveTextFrame.Size.x * WAVE_SPRITE_SCALE);
        GLfloat waveScaledHeight = (GLfloat)(UI_Obj->WaveTextFrame.Size.y * WAVE_SPRITE_SCALE);

        // Draw the text
        Coord2D WaveTextStartPos = UI_Obj->WaveTextPos;

        // Calculate the bounding box
        GLfloat xPositionLeft = (WaveTextStartPos.x - waveScaledWidth / 2);
        GLfloat xPositionRight = (WaveTextStartPos.x + waveScaledWidth / 2);
        GLfloat yPositionTop = (WaveTextStartPos.y - waveScaledHeight / 2);
        GLfloat yPositionBottom = (WaveTextStartPos.y + waveScaledHeight / 2);

        // Calculate the UV coordinates for the sprite based on its position in the sprite sheet
        GLfloat xTextureCoord = UI_Obj->WaveTextFrame.Position.x / UI_Obj->WaveTextSheetSize.x;
        GLfloat yTextureCoord = 1.0f - (UI_Obj->WaveTextFrame.Position.y / UI_Obj->WaveTextSheetSize.y);
        GLfloat uWidth = UI_Obj->WaveTextFrame.Size.x / UI_Obj->WaveTextSheetSize.x;
        GLfloat vHeight = UI_Obj->WaveTextFrame.Size.y / UI_Obj->WaveTextSheetSize.y;

        const float BG_DEPTH = -0.99f;

        // Enable blending to apply color tinting
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, WaveTexture);
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

        /* Wave number */
        // Draw the number
        uint32_t WaveNum = UI_Obj->WaveNum;
        Coord2D WaveNumStartPos = UI_Obj->WaveNumPos;
        while (WaveNum > 0)
        {
            uint8_t Digit = WaveNum % 10;

            // Draw the digit

            // Calculate the bounding box
            GLfloat xPositionLeft = (WaveNumStartPos.x - scaledWidth / 2);
            GLfloat xPositionRight = (WaveNumStartPos.x + scaledWidth / 2);
            GLfloat yPositionTop = (WaveNumStartPos.y - scaledHeight / 2);
            GLfloat yPositionBottom = (WaveNumStartPos.y + scaledHeight / 2);

            // Calculate the UV coordinates for the sprite based on its position in the sprite sheet
            GLfloat xTextureCoord = UI_Obj->NumberFrames[Digit].Position.x / UI_Obj->NumberSheetSize.x;
            GLfloat yTextureCoord = 1.0f - (UI_Obj->NumberFrames[Digit].Position.y / UI_Obj->NumberSheetSize.y);
            GLfloat uWidth = UI_Obj->NumberFrames[Digit].Size.x / UI_Obj->NumberSheetSize.x;
            GLfloat vHeight = UI_Obj->NumberFrames[Digit].Size.y / UI_Obj->NumberSheetSize.y;

            const float BG_DEPTH = -0.99f;

            // Enable blending to apply color tinting
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);

            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, ScoreTexture);
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

            WaveNumStartPos.x -= UI_Obj->NumberGap;
            WaveNum /= 10;
        }
    }

    if (UI_Obj->LevelNumber == 2)
    {
        /* Highscore Vals */
        // Scale the number sprite
        GLfloat scaledWidth = (GLfloat)(UI_Obj->NumberFrames[0].Size.x * UI_Obj->ScoreSpriteScale);
        GLfloat scaledHeight = (GLfloat)(UI_Obj->NumberFrames[0].Size.y * UI_Obj->ScoreSpriteScale);
        Coord2D ScoreStartPos = UI_Obj->HighScoresPos;

        for (int i = 0; i < 3; i++)
        {
            // Draw the score
            ScoreStartPos.x = UI_Obj->HighScoresPos.x;
            uint32_t Score = UI_Obj->HighScoreVals[i];

            if (Score == 0)
            {
                // Calculate the bounding box
                GLfloat xPositionLeft = (ScoreStartPos.x - scaledWidth / 2);
                GLfloat xPositionRight = (ScoreStartPos.x + scaledWidth / 2);
                GLfloat yPositionTop = (ScoreStartPos.y - scaledHeight / 2);
                GLfloat yPositionBottom = (ScoreStartPos.y + scaledHeight / 2);

                // Calculate the UV coordinates for the sprite based on its position in the sprite sheet
                GLfloat xTextureCoord = UI_Obj->NumberFrames[0].Position.x / UI_Obj->NumberSheetSize.x;
                GLfloat yTextureCoord = 1.0f - (UI_Obj->NumberFrames[0].Position.y / UI_Obj->NumberSheetSize.y);
                GLfloat uWidth = UI_Obj->NumberFrames[0].Size.x / UI_Obj->NumberSheetSize.x;
                GLfloat vHeight = UI_Obj->NumberFrames[0].Size.y / UI_Obj->NumberSheetSize.y;

                const float BG_DEPTH = -0.98f;

                // Enable blending to apply color tinting
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);

                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, ScoreTexture);
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

                ScoreStartPos.x -= UI_Obj->NumberGap;
            }

            while (Score > 0)
            {
                uint8_t Digit = Score % 10;

                // Draw the digit

                // Calculate the bounding box
                GLfloat xPositionLeft = (ScoreStartPos.x - scaledWidth / 2);
                GLfloat xPositionRight = (ScoreStartPos.x + scaledWidth / 2);
                GLfloat yPositionTop = (ScoreStartPos.y - scaledHeight / 2);
                GLfloat yPositionBottom = (ScoreStartPos.y + scaledHeight / 2);

                // Calculate the UV coordinates for the sprite based on its position in the sprite sheet
                GLfloat xTextureCoord = UI_Obj->NumberFrames[Digit].Position.x / UI_Obj->NumberSheetSize.x;
                GLfloat yTextureCoord = 1.0f - (UI_Obj->NumberFrames[Digit].Position.y / UI_Obj->NumberSheetSize.y);
                GLfloat uWidth = UI_Obj->NumberFrames[Digit].Size.x / UI_Obj->NumberSheetSize.x;
                GLfloat vHeight = UI_Obj->NumberFrames[Digit].Size.y / UI_Obj->NumberSheetSize.y;

                const float BG_DEPTH = -0.98f;

                // Enable blending to apply color tinting
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);

                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, ScoreTexture);
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

                ScoreStartPos.x -= UI_Obj->NumberGap;
                Score /= 10;
            }
            ScoreStartPos.y += HIGHSCORES_GAP;
        }
    }
}

void UIUpdate(Object* obj, uint32_t milliseconds)
{
    assert(obj != NULL);
    UIElements* UI_Obj = (UIElements*)obj;
    assert(UI_Obj != NULL);

    if (obj->IsActive == false || obj->IsPaused == true) return;

    objSetRandomColor(obj, milliseconds);
}

void UIColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType)
{
}