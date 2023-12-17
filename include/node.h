#pragma once
#include "raylib.h"

inline void DrawNode(Texture texture, const Vector2& pos, float rotation, const Vector2& size, Color tint, const Vector2& centerOffset)
{
	Rectangle dest = { pos.x, pos.y, (float)texture.width, (float)texture.height };
	Vector2 center = { texture.width / 2.0f + centerOffset.x, texture.height / 2.0f + centerOffset.y };

	if (size.x > 0)
	{
		float aspect = dest.height / dest.width;
		dest.width = size.x;
		dest.height = size.y * aspect;
		center.x = size.x * 0.5f + centerOffset.x;
		center.y = size.y * 0.5f + centerOffset.y;
	}
	DrawTexturePro(texture, Rectangle{ 0.0f, 0.0f, dest.width, dest.height }, dest, center, rotation, tint);
}

class Node {
public:
    virtual ~Node() = default;
    virtual void Init() = 0;
    virtual void Update();
    virtual void Draw() = 0;
    virtual bool Collide(const Node& other);

    Vector2 position = {};
    float orientation = {};

    Vector2 velocity = {};
    float rotationalVelocity = {};
    float radius = {};
};