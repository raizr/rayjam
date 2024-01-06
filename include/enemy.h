#pragma once
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
    Timer timer;
    Timer obstacleTimer;

    float enemyRadius = 500.0f;
    bool isFound = false;
    Vector2 enemyDirection = {};
};
