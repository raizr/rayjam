#pragma once
#include "node.h"

class PowerUp : public Node
{
public:
	enum class PowerType
	{
		Shot = 0,
		Shield,
		Life
	};

	PowerUp();
	void Update() override;
	void Draw() override;

	bool Collide(Node& other) override;
    static void Create(const Vector2& pos);

    PowerType typePowerUp = PowerType::Shot;
	Texture texture;
};
