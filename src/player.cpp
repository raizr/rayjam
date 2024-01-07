#include "player.h"
#include <string>

#include "core.h"
#include "explosion.h"
#include "missle.h"
#include "raymath.h"
#include "resource.h"
#include "scene_manager.h"

constexpr float shieldHitMaxLife = 0.35f;
constexpr float breakingFriction = 0.025f;

void Player::Init()
{
    type = NodeType::PLAYER;
    life = 5;
    radius = 40.0f;
    isAlive = true;
    shotSpeedMultiplyer = 1;
    shield = maxShield;
    power = maxPower;
    std::string dir = GetWorkingDirectory();
    shipTexture = Resources::ship;
    thrust = Resources::thrust;
    shieldTexture = Resources::shieldTexture;
    thrustLoop = LoadAsepriteTag(thrust, "loop");
    position = Vector2Zero();
}

void Player::Update()
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

    bool wantThrust = IsKeyDown(KEY_W) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
    bool wantBoost = (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) /*&& wantThrust*/;
    bool wantShoot = IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    bool wantBreak = IsKeyDown(KEY_S);

    axisThrust = 0.0f;
    if (wantThrust)
    {
        axisThrust = 1.0f;
    }
    else if (IsGamepadAvailable(0))
    {
        axisThrust = Clamp(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) + 1.0f, 0.0f, 1.0f);
        wantBoost = wantBoost || (IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN));
        wantShoot = wantShoot || GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.125f;
        wantBreak = wantBreak || IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT);
    }

    // handle rotation by mouse,keyboard, or gamepad
    if (IsCursorOnScreen() && Vector2LengthSqr(GetMouseDelta()) > 0)
    {
        Vector2 mouseVec = Vector2Normalize(Vector2Subtract(GetMousePosition(),
            Vector2Scale({ (float)GetScreenWidth() , (float)GetScreenHeight() }, 0.5f)));

        if (Vector2LengthSqr(mouseVec) > 0)
        {
            orientation = atan2f(mouseVec.y, mouseVec.x) * RAD2DEG + 90;
        }
    }
    else
    {
        float rotation = 360 * core::Core::getInstance()->GetDeltaTime();
        float input = 0;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D))
        {
            if (IsKeyDown(KEY_A))
            {
                input -= 1;
            }
            if (IsKeyDown(KEY_D))
            {
                input += 1;
            }
        }
        else if (IsGamepadAvailable(0))
        {
            input = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
        }
        orientation += input * rotation;
    }
    
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

    // decay the reload timer
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
        isThrusting = false;
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

        Vector2 shotVel = Vector2Add({0.1f, 0.1f}, Vector2Scale(shipVector, 1500));
        Missle::Create(shotPos, shotVel, orientation, type == NodeType::PLAYER, missleColor);
    }
}

void Player::Draw()
{
    if (isAlive)
    {
        if (isThrusting)
        {
            Vector2 offset = { 0, -50.0f };
            if (axisThrust > 0.0f)
            {
                if (axisThrust < 0.3f)
                {
                    DrawFrameAnim(thrust, position, 0, orientation,{ -1.0f, -1.0f }, WHITE, offset);
                } else if (axisThrust < 0.7f)
                {
                    DrawFrameAnim(thrust, position, 1, orientation, { -1.0f, -1.0f }, WHITE, offset);
                }
                else if (axisThrust < 1.0f)
                {
                    DrawFrameAnim(thrust, position, 2, orientation, { -1.0f, -1.0f }, WHITE, offset);
                }
                else
                {
                    DrawFrameAnim(thrustLoop, position, orientation, { -1.0f, -1.0f }, WHITE, offset);
                }
            }
        }
        DrawNode(shipTexture, position, orientation, { -1.0f, -1.0f }, WHITE, { 0.0f, 0.0f });
    }
    if (shield > 0)
    {
        BeginBlendMode(BLEND_ADDITIVE);
        DrawNode(shieldTexture, position, orientation, Vector2{ -1.0f, -1.0f  }, WHITE, { 0.0f, 0.0f });
        EndBlendMode();
    }
}

const Vector2& Player::GetPosition() const
{
    return position;
}

const Vector2& Player::GetDirection() const
{
    return shipVector;
}

bool Player::Collide(Missle& other)
{
    bool hit = Node::Collide(other);
    if (hit && !other.byPlayer)
    {
        OnHit();
    }

    return hit;
}

void Player::OnHit()
{
    Explosion::Create(position, 1);
    if (shield > 0)
    {
        shield--;
        return;
    }
    life--;
    if (life <= 0)
    {
        Explosion::Create(position, radius);
        isAlive = false;
        if (type == NodeType::PLAYER)
        {
            core::Core::getInstance()->OnLose();
        }
    }
}