#pragma once
#include "node.h"


class Asteroid: public Node {
public:
    Asteroid();
    void Update() override;
    void Draw() override;
    bool Collide(Node& other) override;
    Texture texture = {};
    float scale = 1.0f;
};