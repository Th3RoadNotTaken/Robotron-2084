#pragma once
#include "baseTypes.h"

typedef struct SpriteFrame_t
{
	Coord2D Position;
	Coord2D Size;
} SpriteFrame;

typedef struct SpriteAnimation_t
{
	SpriteFrame* AnimationFrames;
	uint8_t NumFrames;
	uint8_t CurrentFrame;
	uint32_t AccumulatedTime;
	uint32_t FrameDuration;
} SpriteAnimation;

SpriteAnimation* InitAnimation(SpriteFrame* AnimFrames, uint8_t NumFrames, uint32_t AccumulatedTime, uint32_t FrameDuration);

void PlayAnimation(SpriteAnimation* Anim, uint32_t DeltaTime);
bool PlayAnimationOneShot(SpriteAnimation* Anim, uint32_t DeltaTime);

void ResetAnimation(SpriteAnimation* Anim);

SpriteFrame* GetCurrentFrame(SpriteAnimation* Anim);

SpriteFrame* GetFrame(SpriteAnimation* Anim, uint8_t FrameNumber);