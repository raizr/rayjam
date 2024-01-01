#pragma once
#include "player.h"
#include "timer.h"


class Enemy: public Player {
public:
    void Init() override;
    void Update() override;

    Timer timer;
};
