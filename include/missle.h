#pragma once
#include "node.h"

class Missle: public Node {
public:

	Missle();
	void Draw() override;
	void Update() override;
	static void Create(const Vector2& pos, const Vector2& velocity, float orientation, bool byPlayer = false);

	Color tint = WHITE;
	float lifeTime = 0;
    Texture texture = {};
	bool isAlive = false;
	bool byPlayer = false;
};