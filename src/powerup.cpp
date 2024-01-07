#include "powerup.h"

#include "player.h"
#include "pool.h"
#include "resource.h"
#include "scene_manager.h"

constexpr float PowerupRadius = 55;

PowerUp::PowerUp()
{
	radius = 20.f;
}

void PowerUp::Update()
{
	Node::Update();
}

void PowerUp::Draw()
{
	if (!isAlive)
		return;

	DrawNode(texture, position, orientation, { -1.0f, -1.0f }, WHITE, { 0.0f, 0.0f });
}

bool PowerUp::Collide(Node& other)
{
	if (Node::Collide(other))
	{
		//Sounds::PlaySoundEffect(Sounds::Upgrade);
		Player& player = scene::SceneManager::getInstance()->GetPlayer();
		switch (typePowerUp)
		{
			case PowerType::Shot:
				player.shotSpeedMultiplyer += 0.10f;
				break;

			case PowerType::Shield:
				player.shield = player.maxShield;
				break;

			case PowerType::Life:
				if (player.life < 5)
				{
				    player.life++;
				}
				break;
		}

		isAlive = false;
		return true;
	}
	return false;
}


void PowerUp::Create(const Vector2& pos)
{
	PowerUp* slot = nullptr;
	for (auto& explosion : Pool::powerups)
	{
		if (!explosion.isAlive)
		{
			slot = &explosion;
			break;
		}
	}

	if (!slot)
	{
		PowerUp element;
		element.Init();
		Pool::powerups.emplace_back(std::move(element));
		slot = &Pool::powerups.back();
	}
	auto index = GetRandomValue(0, 2);
	slot->typePowerUp = (PowerType)index;
	slot->texture = Resources::powerUpSprites[index];
	slot->isAlive = true;
	slot->position = pos;
	Vector2 velocity = { GetRandomValueF(-50,50) ,GetRandomValueF(-50,50) };
	slot->velocity = velocity;
	slot->orientation = 0;
	slot->rotationalVelocity = (float)GetRandomValue(-360, 360);
}