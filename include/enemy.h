#pragma once
#include <array>

#include "missle.h"
#include "player.h"
#include "timer.h"


inline float GetRandomValueF(float min, float max)
{
    if (min > max)
    {
        float tmp = max;
        max = min;
        min = tmp;
    }
    double param = rand() / (double)RAND_MAX;
    return float(min + (max - min) * param);
}

class Enemy: public Player {
public:
    void Init() override;
    void Update() override;
    void Draw() override;
    bool Collide(Missle& missle) override;
    Timer timer = {};
    Timer obstacleTimer = {};

    float enemyRadius = 10000.0f;
    bool isFound = false;
    Vector2 enemyDirection = {};
    bool isMooving = false;
    std::array<Color, 7> missleColors = {
        BLUE,
        GREEN,
        YELLOW,
        ORANGE,
        GOLD,
        PINK,
        MAROON
    };
};
