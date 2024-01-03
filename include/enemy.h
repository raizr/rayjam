#pragma once
#include "missle.h"
#include "player.h"
#include "timer.h"

class Enemy: public Player {
public:
    void Init() override;
    void Update() override;
    void Draw() override;
    bool Collide(Missle& missle) override;
    Timer timer;

    float enemyRadius = 500.0f;
    bool isFound = false;
    Vector2 enemyDirection = {};
};
