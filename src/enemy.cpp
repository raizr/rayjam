#include "enemy.h"

#include "core.h"
#include "explosion.h"
#include "missle.h"
#include "pool.h"
#include "raymath.h"
#include "resource.h"
#include "scene_manager.h"

constexpr float breakingFriction = 0.025f;
constexpr float baseReloadTime = 0.5f;


void Enemy::Init()
{
    type = NodeType::ENEMY;
    std::string dir = GetWorkingDirectory();
    shipTexture = Resources::shipEnemy[GetRandomValue(0, Resources::shipEnemy.size() - 1)];
    thrust = Resources::thrust;
    thrustLoop = LoadAsepriteTag(thrust, "loop");
    auto curLevel = core::Core::getInstance()->GetCurrentLevel();
    radius = 40.0f;
    baseReloadTime = 5.0f - 0.1f * curLevel;
    life = GetRandomValue(1 + curLevel, 3 + curLevel);
    axisThrust = GetRandomValueF(0.3f + 0.05f * curLevel, 0.5f + 0.05f * curLevel);
    StartTimer(&timer, 2.0);
    orientation = GetRandomValueF(0.0f, 360.f);
    missleColor = missleColors[GetRandomValue(0, missleColors.size() - 1)];
}

void Enemy::Update()
{
    if (!isAlive)
    {
        return;
    }

    UpdateAsepriteTag(&thrustLoop);
    if (shield > maxShield)
    {
        shield = maxShield;
    }
    
    if (!isFound)
    {
        if (TimerDone(timer) && !isMooving)
        {
            orientation = GetRandomValueF(orientation - 10.f, orientation + 10.f);
            StartTimer(&timer, 1.0);
        }
    }
    bool wantBoost = false;
    bool wantShoot = false;
    bool wantBreak = false;
    auto& player = scene::SceneManager::getInstance()->GetPlayer();
    isFound = CheckCollisionCircles(position, enemyRadius, player.position, player.radius) && player.isAlive;
    if (isFound)
    {
        orientation = -Vector2LineAngle((position), (player.position)) * RAD2DEG + 90;
        auto distance = Vector2Distance(position, player.position);
        if (distance <= 100)
        {
            axisThrust = 0.0f;
            velocity = {
                0.1f + distance / 100.f,
                0.1f + distance / 100.f
            };
        }
        else
        {
            auto curLevel = core::Core::getInstance()->GetCurrentLevel();
            axisThrust = GetRandomValueF(0.3f + 0.05f * curLevel, 0.5f + 0.05f * curLevel);
        }
        if (CheckCollisionCircles(position, 500, player.position, player.radius))
        {
            wantShoot = true;
        }
    }
    else
    {
        position = Vector2Add(position, Vector2Negate(player.position));
    }
    
    while (orientation > 180)
    {
        orientation -= 360;
    }
    while (orientation < -180)
    {
        orientation += 360;
    }
    if (!wantBoost)
    {
        boost = false;
    }
    else if (wantBoost && power > maxPower / 4)
    {
        boost = true;
    }
    else if (power <= 1)
    {
        boost = false;
    }

    if (boost)
    {
        power -= core::Core::getInstance()->GetDeltaTime() * 400;
    }
    else if (power < maxPower)
    {
        power += core::Core::getInstance()->GetDeltaTime() * 20;
    }

    if (power < 0)
    {
        power = 0;
    }
    if (power > maxPower)
    {
        power = maxPower;
    }

    reload -= core::Core::getInstance()->GetDeltaTime() * shotSpeedMultiplyer;
    shipVector = Vector2{ sinf(orientation * DEG2RAD), -cosf(orientation * DEG2RAD) };
    speed = maxThrust * axisThrust * core::Core::getInstance()->GetDeltaTime();

    if (boost)
    {
        speed *= boostMultiplyer;
    }

    if (axisThrust > 0.0f)
    {
        velocity = Vector2Add(velocity, Vector2Scale(shipVector, speed));
        isThrusting = true;
    }

    float frictionScale = 90;
    if (wantBreak)
    {
        frictionScale *= breakingFriction * maxThrust;
    }

    Vector2 normVel = Vector2Normalize(velocity);
    Vector2 friction = Vector2Scale(normVel, -frictionScale * GetFrameTime());
    if (Vector2LengthSqr(friction) >= Vector2LengthSqr(velocity))
    {
        velocity = { 0, 0 };
    }
    else
    {
        velocity = Vector2Add(velocity, friction);
    }

    float maxSpeed = boost ? 5000.0f : 2000.0f;
    if (Vector2LengthSqr(velocity) > maxSpeed * maxSpeed)
    {
        velocity = Vector2Scale(normVel, maxSpeed);
    }
    position = Vector2Add(position, Vector2Scale(velocity, core::Core::getInstance()->GetDeltaTime()));
    if (wantShoot && reload <= 0)
    {
        reload = baseReloadTime;
        Vector2 shotPos = Vector2Add(position, Vector2Scale(shipVector, radius * 1.0f));
        Vector2 shotVel = Vector2Add(velocity, Vector2Scale(shipVector, 1500));
        Missle::Create(shotPos, shotVel, orientation, false, missleColor);
        PlaySoundEffect(Resources::shoot[GetRandomValue(0, Resources::shoot.size() - 1)]);
    }
}

void Enemy::Draw()
{
    Player::Draw();
    /*Vector2 ray = Vector2Add(position, Vector2Scale(shipVector, 100.0f));
    DrawLineV(position, ray, RED);*/
}

bool Enemy::Collide(Missle& other)
{
    bool hit = Node::Collide(other);
    if (hit && other.byPlayer)
    {
        OnHit();
    }

    return hit;
}

