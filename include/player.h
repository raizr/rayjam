#pragma once
#include "node.h"

static constexpr float NominalShield = 1000;
static constexpr float NominalShieldRecharge = 2;
static constexpr float NominalPower = 1000;
static constexpr float NominalThrust = 400;
static constexpr float NominalBoostMultiplyer = 3;

class Player: public Node {
public:
	void Init() override;
    void Update() override;
    void Draw() override;
	const Vector2& GetPosition() const;
private:

	Texture shipTexture = {};

	float reload = 0;

	bool isThrusting = false;
	bool boost = false;

	float shotSpeedMultiplyer = 1;

	float maxShield = NominalShield;
	float shield = maxShield;

	float maxPower = NominalPower;
	float Power = maxPower;

	float maxThrust = NominalThrust;
	float boostMultiplyer = NominalBoostMultiplyer;

	float shieldRecharge = NominalShieldRecharge;
	float shieldHitAngle = 0;
	float shieldHitLifetime = -1;

	int score = 0;
	bool isAlive = true;
};
