#pragma once
#include "node.h"
#include "raylib.h"
#include "missle.h"
#include "raylib-aseprite.h"

static constexpr float NominalPower = 1000;
static constexpr float NominalThrust = 400;
static constexpr float NominalBoostMultiplyer = 3;



inline void DrawFrameAnim(Aseprite aseprite, const Vector2& pos, int frame, float rotation, const Vector2& size, Color tint, const Vector2& centerOffset)
{
	Rectangle dest = { pos.x, pos.y, (float)aseprite.ase->w, (float)aseprite.ase->h };
	Vector2 center = { aseprite.ase->w / 2.0f + centerOffset.x, aseprite.ase->h / 2.0f + centerOffset.y };

	if (size.x > 0)
	{
		float aspect = dest.height / dest.width;
		dest.width = size.x;
		dest.height = size.y * aspect;
		center.x = size.x * 0.5f + centerOffset.x;
		center.y = size.y * 0.5f + centerOffset.y;
	}
	DrawAsepritePro(aseprite,frame , dest, center, rotation, tint);
}

inline void DrawFrameAnim(AsepriteTag aseprite, const Vector2& pos, float rotation, const Vector2& size, Color tint, const Vector2& centerOffset)
{
	Rectangle dest = { pos.x, pos.y, (float)aseprite.aseprite.ase->w, (float)aseprite.aseprite.ase->h };
	Vector2 center = { aseprite.aseprite.ase->w / 2.0f + centerOffset.x, aseprite.aseprite.ase->h / 2.0f + centerOffset.y };

	if (size.x > 0)
	{
		float aspect = dest.height / dest.width;
		dest.width = size.x;
		dest.height = size.y * aspect;
		center.x = size.x * 0.5f + centerOffset.x;
		center.y = size.y * 0.5f + centerOffset.y;
	}
	DrawAsepriteTagPro(aseprite, dest, center, rotation, tint);
}

class Player: public Node {
public:
	void Init() override;
    void Update() override;
    void Draw() override;
	const Vector2& GetPosition() const;
    const Vector2& GetDirection() const;
	virtual bool Collide(Missle& other);
    void OnHit();

    Texture shipTexture = {};
    Texture shieldTexture = {};
	Aseprite thrust = {};
	AsepriteTag thrustLoop = {};
	Color missleColor = RED;
	float reload = 0;

	bool isThrusting = false;
	bool boost = false;

	float shotSpeedMultiplyer = 1;

	float maxShield = 5;
	float shield = maxShield;

	float maxPower = NominalPower;
	float power = maxPower;

	float maxThrust = NominalThrust;
	float boostMultiplyer = NominalBoostMultiplyer;
	
	float speed = {};
	float axisThrust = 0.0f;
	float baseReloadTime = 0.5f;
	int score = 0;
	int life = 5;
	Vector2 shipVector;
};
