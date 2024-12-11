#include "sprite.h"
#include <stdlib.h>
#include "assert.h"

SpriteAnimation* InitAnimation(SpriteFrame* AnimFrames, uint8_t NumFrames, uint32_t AccumulatedTime, uint32_t FrameDuration)
{
    assert(AnimFrames != NULL);

	SpriteAnimation* Animation = malloc(sizeof(SpriteAnimation));
    assert(Animation != NULL);

	Animation->AnimationFrames = AnimFrames;
    Animation->NumFrames = NumFrames;
	Animation->CurrentFrame = 0;
    Animation->AccumulatedTime = AccumulatedTime;
	Animation->FrameDuration = FrameDuration;

	return Animation;
}

void PlayAnimation(SpriteAnimation* Anim, uint32_t DeltaTime)
{
    assert(Anim != NULL);

    Anim->AccumulatedTime += DeltaTime;

    // Check if enough time has passed to advance the frame
    if (Anim->AccumulatedTime >= Anim->FrameDuration)
    {
        Anim->AccumulatedTime = 0; // Reset the time counter
        if (Anim->CurrentFrame + 1 >= Anim->NumFrames)
        {
            Anim->CurrentFrame = 0;
        }
        else
            Anim->CurrentFrame++; // Move to the next frame
    }
}

bool PlayAnimationOneShot(SpriteAnimation* Anim, uint32_t DeltaTime)
{
    assert(Anim != NULL);

    if (Anim->CurrentFrame >= Anim->NumFrames - 1)
    {
        Anim->CurrentFrame = 0;
        return false;
    }

    Anim->AccumulatedTime += DeltaTime;

    // Check if enough time has passed to advance the frame
    if (Anim->AccumulatedTime >= Anim->FrameDuration)
    {
        Anim->AccumulatedTime = 0; // Reset the time counter

        // Advance the frame
        Anim->CurrentFrame++;

        // Check if we reached the last frame after advancing
        if (Anim->CurrentFrame >= Anim->NumFrames - 1)
        {
            Anim->CurrentFrame = 0;
            return false;
        }
    }

    return true;
}

void ResetAnimation(SpriteAnimation* Anim)
{
    assert(Anim != NULL);

    Anim->AccumulatedTime = 0;
    Anim->CurrentFrame = 0;
}

SpriteFrame* GetCurrentFrame(SpriteAnimation* Anim)
{
    assert(Anim != NULL);

    if (Anim->CurrentFrame >= Anim->NumFrames) {
        Anim->CurrentFrame = 0; // Loop back to the first frame
    }
    return &Anim->AnimationFrames[Anim->CurrentFrame];
}

SpriteFrame* GetFrame(SpriteAnimation* Anim, uint8_t FrameNumber)
{
    assert(Anim != NULL);

    if (FrameNumber < 0 || FrameNumber >= Anim->NumFrames)
        return NULL;

    return &Anim->AnimationFrames[FrameNumber];
}