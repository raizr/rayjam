#include "enemy.h"

#include "core.h"
#include "explosion.h"
#include "missle.h"
#include "raymath.h"
#include "resource.h"
#include "scene_manager.h"

constexpr float breakingFriction = 0.025f;
constexpr float baseReloadTime = 0.5f;
using namespace std::string_literals;

inline float GetRandomValueF(float min, float max)
{
    if (min > max)
    {
        float tmp = max;
        max = min;
        min = tmp;
    }
    double param = rand() / (double)RAND_MAX;
    return float(min + (max - min) * param);
}

void Enemy::Init()
{
    type = NodeType::ENEMY;
    std::string dir = GetWorkingDirectory();
    shipTexture = Resources::ship;
    thrust = Resources::thrust;
    thrustLoop = LoadAsepriteTag(thrust, "loop");
    radius = 40.0f;

    axisThrust = GetRandomValueF(0.5f, 0.8f);
    position = {
        GetRandomValueF(-10000.0f, 10000.f),
        GetRandomValueF(-10000.0f, 10000.f)
    };
    StartTimer(&timer, 5.0);
    orientation = GetRandomValueF(0.0f, 360.f);
}

void Enemy::Update()
{
    shieldHitLifetime -= core::Core::getInstance()->GetDeltaTime();

    if (!isAlive)
    {
        return;
    }
    UpdateAsepriteTag(&thrustLoop);
    shield += core::Core::getInstance()->GetDeltaTime() * shieldRecharge;
    if (shield > maxShield)
    {
        shield = maxShield;
    }
    if (!isFound)
    {
        if (TimerDone(timer))
        {
            orientation = GetRandomValueF(orientation - 10.f, orientation + 10.f);
            StartTimer(&timer, 5.0);
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
        axisThrust = distance / enemyRadius;
        wantShoot = true;
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
    if (isFound)
    {
        //shipVector = enemyDirection;
    }
    else
    {
    }
        shipVector = Vector2{ sinf(orientation * DEG2RAD),
            -cosf(orientation * DEG2RAD) };
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
        Missle::Create(shotPos, shotVel, orientation);
        //Sounds::PlaySoundEffect(Sounds::Shot);
    }
}

void Enemy::Draw()
{
    DrawCircleLines(position.x, position.y, enemyRadius, RED);
    Player::Draw();

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

