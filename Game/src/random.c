#include <stdlib.h>
#include "random.h"

/// @brief Return a random floating point value in the specified range
/// @param min 
/// @param max 
/// @return 
float randGetFloat(float min, float max)
{
    int r = rand();
    float rPct = (float)r/(float)RAND_MAX;

    return (rPct * (max - min)) + min;
}

/// @brief Return a random 32-bit value in the specified range
/// @param min 
/// @param max 
/// @return 
int32_t randGetInt(int32_t min, int32_t max)
{
    int r = rand();

    r %= (max - min);
    r += min;

    return r;
}