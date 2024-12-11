#pragma once
#include "baseTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SOUND_NOSOUND -1

bool soundInit(int32_t maxSounds);
bool soundShutdown();
int32_t soundLoad(const char* filename);
void soundUnload(int32_t soundId);
void soundPlay(int32_t soundId);
void soundStop(int32_t soundId);

#ifdef __cplusplus
}
#endif