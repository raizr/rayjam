#include "enemy.h"

#include "core.h"
#include "missle.h"
#include "raymath.h"

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
    std::string dir = GetWorkingDirectory();
    shipTexture = LoadTexture((dir + "/sprites/ship.png"s).c_str());
    thrust = LoadAseprite((dir + "/sprites/thrust.aseprite"s).c_str());
    thrustLoop = LoadAsepriteTag(thrust, "loop");


    axisThrust = GetRandomValueF(0.3f, 1.0f);
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

    if (TimerDone(timer))
    {
        orientation = GetRandomValueF(orientation - 10.f, orientation + 10.f);
        StartTimer(&timer, 5.0);
        
    }
    // gather our input states
    bool wantThrust = false;
    bool wantBoost = false;
    bool wantShoot = false;
    bool wantBreak = false;
    
    while (orientation > 180)
    {
        orientation -= 360;
    }
    while (orientation < -180)
    {
        orientation += 360;
    }
    // boost if we can boost
    if (!wantBoost)
    {
        boost = false;
    }
    else if (wantBoost && Power > maxPower / 4)
    {
        boost = true;
    }
    else if (Power <= 1)
    {
        boost = false;
    }

    if (boost)
    {
        Power -= core::Core::getInstance()->GetDeltaTime() * 400;
    }
    else if (Power < maxPower)
    {
        Power += core::Core::getInstance()->GetDeltaTime() * 20;
    }

    if (Power < 0)
        Power = 0;
    if (Power > maxPower)
        Power = maxPower;

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
    else
    {
        //isThrusting = false;
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
