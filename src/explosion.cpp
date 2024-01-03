#include "explosion.h"

#include <string>
#include "core.h"
#include "pool.h"
#include "raymath.h"
#include "resource.h"

constexpr float maxLifetime = 0.5f;
using namespace std::string_literals;

void Explosion::Init()
{
    Node::Init();
	type = NodeType::EXPLOSION;
	std::string dir = GetWorkingDirectory();
	animSprite = Resources::animExplosionSprite;
	animSpriteLoop = {
		LoadAsepriteTag(animSprite[0], "loop"),
		LoadAsepriteTag(animSprite[1], "loop")
	};
}

void Explosion::Draw()
{
	if (!isAlive)
	{
		return;
	}

	float param = lifeTime / maxLifetime;
	Color c = ColorAlpha(WHITE, param);
	int i = 0;
	for (const auto& particle : particles)
	{
		DrawFrameAnim(animSpriteLoop[i % 2],
			    Vector2Add(position, particle.position),
			    orientation,
			    {-1.0f, -1.0f}, c, Vector2{0.0f, 0.0f});
		i++;
	}
}

void Explosion::Update()
{
	if (!isAlive)
	{
		return;
	}
	for (auto& anim : animSpriteLoop)
	{
	    UpdateAsepriteTag(&anim);
	}
	lifeTime -= core::Core::getInstance()->GetDeltaTime();

	if (lifeTime < 0)
	{
		isAlive = false;
	}

	Node::Update();

	for (auto& particle : particles)
	{
		particle.Update();
	}
}

void Explosion::Create(const Vector2& pos, float size)
{
	Explosion* slot = nullptr;
	for (auto& explosion : Pool::explosions)
	{
		if (!explosion.isAlive)
		{
			slot = &explosion;
			break;
		}
	}

	if (!slot)
	{
		Explosion element;
		element.Init();
		Pool::explosions.emplace_back(std::move(element));
		slot = &Pool::explosions.back();
	}

	slot->isAlive = true;
	slot->position = pos;
	slot->velocity = Vector2{ 0, 0 };
	slot->orientation = 0;
	slot->rotationalVelocity = 0;
	slot->lifeTime = maxLifetime;

	slot->particles.clear();

	int particles = GetRandomValue(1, size);
	
	slot->particles.reserve(particles);
	for (int i = 0; i < particles; i++)
	{
		Node particle;
		particle.orientation = (float)GetRandomValue(0, 180);
		particle.rotationalVelocity = (float)GetRandomValue(180, 720);

		float randomAngle = (float)GetRandomValue(-180, 180) * DEG2RAD;
		float randomSpeed = (float)GetRandomValue(20, 500);

		particle.velocity = Vector2{ cosf(randomAngle) * randomSpeed,
			sinf(randomAngle) * randomSpeed };

		slot->particles.emplace_back(std::move(particle));
	}
}

