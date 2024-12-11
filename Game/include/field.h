#pragma once
#include "object.h"

#define OUTLINE_THICKNESS 3.0f

#ifdef __cplusplus
extern "C" {
#endif

typedef struct field_t Field;

Field* fieldNew(Bounds2D bounds, uint32_t color);
void fieldDelete(Field* field);

void fieldSetColor(Field* field, long color);
long fieldGetColor(const Field* field);

Coord2D fieldGetSize(const Field* field);

#ifdef __cplusplus
}
#endif