#include <Windows.h>
#include <stdlib.h>
#include <assert.h>
#include <gl/GLU.h>
#include "SOIL.h"

#include "baseTypes.h"
#include "face.h"
#include "Object.h"
#include "random.h"

// all of these values are based upon the layout of the PNG
static const char CHARACTER_PAGE[] = "asset/snoods_default.png";
static const int32_t CHARACTER_COUNT = 8;
typedef enum mood_t
{
    MOOD_NORMAL,
    MOOD_HAPPY,
    MOOD_SAD,
    MOOD_ANGRY,

    MOOD_COUNT
} Mood;

typedef struct face_t
{
    Object      obj;

    Coord2D     size;
    uint32_t    character;
    Mood        mood;

    uint32_t    nextUpdate;
} Face;

static GLuint _faceTexture = 0;

// the object vtable for all faces
static void _faceUpdate(Object* obj, uint32_t milliseconds);
static void _faceDraw(Object* obj);
static void _faceColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType);
static ObjVtable _faceVtable = {
    _faceDraw,
    _faceUpdate
};
static ObjCollisionVtable _faceCollVTable = {
	_faceColl
};

static void _faceUpdateMood(Face* face);
static uint32_t _getUpdateTime();

/// @brief one time initialization of textures
void faceInitTextures()
{
    if (_faceTexture == 0)
    {
        _faceTexture = SOIL_load_OGL_texture(CHARACTER_PAGE, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
        assert(_faceTexture != 0);
    }
}

/// @brief Allocates & initializes a face object
/// @param box 
/// @return 
Face* faceNew(Bounds2D box)
{
    Face* face = malloc(sizeof(Face));
    if (face != NULL)
    {
        Coord2D center = boundsGetCenter(&box);
        Coord2D vel = { 0.0f, 0.0f };
        objInitDefault(&face->obj, &_faceVtable, center, vel, DefaultObjType, &_faceCollVTable);

        // extract the dimensions from the bounding box for rendering
        face->size = boundsGetDimensions(&box);

        // initialize the face to one of the characters in our sprite sheet
        face->character = randGetInt(0, CHARACTER_COUNT);
        face->mood = MOOD_NORMAL;

        // setup a random time to update mood
        face->nextUpdate = _getUpdateTime();
    }

    return face;
}

/// @brief Frees up resources from a face object
/// @param face 
void faceDelete(Face* face)
{
    objDeinit(&face->obj);

    free(face);
}

void _faceColl(Object* SourceObject, Object* TargetObject, enum ObjectType SourceObjType, enum ObjectType TargetObjectType)
{
}

/// @brief Object draw handler
/// @param obj 
static void _faceDraw(Object* obj)
{
    Face* face = (Face*)obj;


    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _faceTexture);
    glBegin(GL_TRIANGLE_STRIP);
    {
        // calculate the bounding box
        GLfloat xPositionLeft = (obj->position.x - face->size.x / 2);
        GLfloat xPositionRight = (obj->position.x + face->size.x / 2);
        GLfloat yPositionTop = (obj->position.y - face->size.y / 2);
        GLfloat yPositionBottom = (obj->position.y + face->size.y / 2);

        // find the proper sprite frame from the 8x4 sprite sheet
        float uPerChar = 1.0f / (float)CHARACTER_COUNT;
        float vPerMood = 1.0f / (float)MOOD_COUNT;

        // calculate the starting uv... remember v of 0 is the bottom of the texture
        GLfloat xTextureCoord = face->character * uPerChar;
        GLfloat yTextureCoord = (MOOD_COUNT - face->mood) * vPerMood;

        const float BG_DEPTH = -0.99f;

        // draw the textured quad as a tristrip
        glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);

        // TL
        glTexCoord2f(xTextureCoord, yTextureCoord);
        glVertex3f(xPositionLeft, yPositionTop, BG_DEPTH);

        // BL
        glTexCoord2f(xTextureCoord, yTextureCoord - vPerMood);
        glVertex3f(xPositionLeft, yPositionBottom, BG_DEPTH);

        // TR
        glTexCoord2f(xTextureCoord + uPerChar, yTextureCoord);
        glVertex3f(xPositionRight, yPositionTop, BG_DEPTH);

        // BR
        glTexCoord2f(xTextureCoord + uPerChar, yTextureCoord - vPerMood);
        glVertex3f(xPositionRight, yPositionBottom, BG_DEPTH);

    }
    glEnd();
}

/// @brief Updates the character's mood every so often
/// @param obj 
/// @param milliseconds 
static void _faceUpdate(Object* obj, uint32_t milliseconds)
{
    objDefaultUpdate(obj, milliseconds);

    Face* face = (Face*)obj;

    // check to see if we should update yet
    if (face->nextUpdate > milliseconds)
    {
        face->nextUpdate -= milliseconds;
        return;
    }

    // perform the update & set up the next update time
    _faceUpdateMood(face);
    face->nextUpdate = _getUpdateTime();
}

/// @brief Choose a random mood
/// @param face 
static void _faceUpdateMood(Face* face)
{
    face->mood = (Mood) randGetInt(0, MOOD_COUNT);
}

/// @brief Tuning method for how frequently updates will occur
/// @return 
static uint32_t _getUpdateTime()
{
    const uint32_t MIN_TIME = 500;
    const uint32_t MAX_TIME = 2000;

    return randGetInt(MIN_TIME, MAX_TIME);
}

