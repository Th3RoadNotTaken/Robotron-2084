#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct face_t Face;

void faceInitTextures();
Face* faceNew(Bounds2D box);
void faceDelete(Face* face);

#ifdef __cplusplus
}
#endif