#pragma once
#include "node.h"


class Asteroid: public Node {
public:
    Asteroid();
    void Update() override;
    void Draw() override;
    void CollideWithShip(Node& ship);
    Texture texture = {};
    float scale = 1.0f;
};