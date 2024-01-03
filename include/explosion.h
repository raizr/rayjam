#pragma once

#include "node.h"
#include <vector>
#include <array>
#include "raylib-aseprite.h"

class Explosion : public Node
{
public:
	void Init() override;
	void Draw() override;

	void Update() override;

	static void Create(const Vector2& pos, float size);

	Color tint = WHITE;
	float lifeTime = 0;

	std::array<Aseprite,2> animSprite = {};
	std::array<AsepriteTag, 2> animSpriteLoop = {};
	std::vector<Node> particles;
	bool isAlive = false;
};
