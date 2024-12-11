#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct 
{
    float x;
    float y;
} Coord2D;

typedef struct
{
    Coord2D topLeft;
    Coord2D botRight;
} Bounds2D;

enum Direction
{
    Up,
    Down,
    Left,
    Right,
    UpLeft,
    UpRight,
    DownLeft,
    DownRight,

    DefaultDir
};

enum ObjectType
{
    Character,
    EnemyGrunt,
    EnemyHulk,
    EnemySpheroid,
    EnemyEnforcer,
    EnemyQuark,
    EnemyTank,
    Projectile,
    Obstacle,
    UI,

    DefaultObjType
};

enum ObjectSubType
{
    SimpleObs,
    ComplexObs,
    StaticObs,
    StaticObs2084
};

/// @brief Utility method to get the center point of a Bounds2D
/// @param bounds 
/// @return 
inline Coord2D boundsGetCenter(const Bounds2D* bounds) {
    Coord2D center = { 
        (bounds->topLeft.x + bounds->botRight.x) / 2, 
        (bounds->topLeft.y + bounds->botRight.y) / 2 
    };
    return center;
}

/// @brief Utility method to get the width and height of a Bounds2D
/// @param bounds 
/// @return 
inline Coord2D boundsGetDimensions(const Bounds2D* bounds) {
    Coord2D size = { 
        bounds->botRight.x - bounds->topLeft.x, 
        bounds->botRight.y - bounds->topLeft.y 
    };
    return size;
}
