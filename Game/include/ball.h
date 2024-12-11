#pragma once
#include "baseTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ball_t Ball;

typedef void (*BallCollideCB)(Ball*);
void ballSetCollideCB(BallCollideCB cb);
void ballClearCollideCB();

Ball* ballNew(Bounds2D bounds);
void ballDelete(Ball* ball);

#ifdef __cplusplus
}
#endif