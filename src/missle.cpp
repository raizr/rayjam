#include "missle.h"

#include <string>

#include "core.h"
#include "pool.h"
#include "resource.h"

constexpr float maxLife = 5;

Missle::Missle()
{
	type = NodeType::MISSLE;
	radius = 20.f;
	texture = Resources::missle;
}

void Missle::Draw()
{
	if (!isAlive)
	{
		return;
	}
	DrawNode(texture, position, orientation,
		{ -1.0f, -1.0f }, ColorAlpha(tint, lifeTime / maxLife), {0.0f, 0.0f});
}

void Missle::Update()
{
	if (!isAlive)
	{
		return;
	}

	lifeTime -= core::Core::getInstance()->GetDeltaTime();

	if (lifeTime < 0)
	{
		isAlive = false;
	}

	Node::Update();
}

void Missle::Create(const Vector2& pos, const Vector2& velocity, float orientation, bool byPlayer, Color color)
{
	auto slot = Pool::getMissle();
	slot->isAlive = true;
	slot->position = pos;
	slot->velocity = velocity;
	slot->orientation = orientation;
	slot->lifeTime = maxLife;
	slot->byPlayer = byPlayer;
	slot->tint = color;
}
