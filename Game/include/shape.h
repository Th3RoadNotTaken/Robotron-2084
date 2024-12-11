#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void shapeDrawCircle(float radius, float x, float y, uint8_t r, uint8_t g, uint8_t b, bool filled);
void shapeDrawLine(float startX, float startY, float endX, float endY, uint8_t r, uint8_t g, uint8_t b, float lineThickness);
void shapeDrawRectangle(float topLeftX, float topLeftY, float bottomRightX, float bottomRightY, uint8_t r, uint8_t g, uint8_t b, float rectThickness);

#ifdef __cplusplus
}
#endif