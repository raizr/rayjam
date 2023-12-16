#include "player.h"
#include <string>

#include "core.h"
#include "raymath.h"

constexpr float baseReloadTime = 0.5f;
constexpr float shieldHitMaxLife = 0.35f;
constexpr float breakingFriction = 0.025f;

void Player::Init()
{
    std::string dir = GetWorkingDirectory();
    shipTexture = LoadTexture((dir + std::string("/sprites/ship.png")).c_str());
}

void Player::Update()
{

    // decay the shield hit time (even if we are dead, so it animates)
    shieldHitLifetime -= core::Core::getInstance()->GetDeltaTime();

    if (!isAlive)
    {
        return;
    }

    // add some shield power back
    shield += core::Core::getInstance()->GetDeltaTime() * shieldRecharge;
    if (shield > maxShield)
    {
        shield = maxShield;
    }


    // gather our input states
    bool wantThrust = IsKeyDown(KEY_W) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
    bool wantBoost = (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) /*&& wantThrust*/;
    bool wantShoot = IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    bool wantBreak = IsKeyDown(KEY_S);
    float axisThrust = 0.0f;
    if (wantThrust)
    {
        axisThrust = 1.0f;
    }
    else if (IsGamepadAvailable(0))
    {
        axisThrust = Clamp(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) + 1.0f, 0.0f, 1.0f);
        //wantThrust = wantThrust || GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.125f;
        wantBoost = wantBoost || (IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)/* && wantThrust*/);
        wantShoot = wantShoot || GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.125f;
        wantBreak = wantBreak || IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT);
    }

    // handle rotation by mouse,keyboard, or gamepad
    if (Vector2LengthSqr(GetMouseDelta()) > 0)
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
                input -= 1;
            if (IsKeyDown(KEY_D))
                input += 1;
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

    // decay the reload timer
    reload -= core::Core::getInstance()->GetDeltaTime() * shotSpeedMultiplyer;

    // turn our angle into a vector so we can see what way we are going
    Vector2 shipVector = Vector2{ sinf(orientation * DEG2RAD), -cosf(orientation * DEG2RAD) };

    // see how much we could move this frame
    float speed = maxThrust * axisThrust * core::Core::getInstance()->GetDeltaTime();

    if (boost)
    {
        speed *= boostMultiplyer;
    }

    // add our desired thrust vector to our current vector, this is what gives us intertia
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
        velocity = { 0, 0 };
    else
        velocity = Vector2Add(velocity, friction);

    float maxSpeed = boost ? 5000.0f : 2000.0f;
    if (Vector2LengthSqr(velocity) > maxSpeed * maxSpeed)
        velocity = Vector2Scale(normVel, maxSpeed);

    position = Vector2Add(position, Vector2Scale(velocity, core::Core::getInstance()->GetDeltaTime()));

    if (wantShoot && reload <= 0)
    {
        reload = baseReloadTime;

        Vector2 shotPos = Vector2Add(position, Vector2Scale(shipVector, radius * 1.0f));

        Vector2 shotVel = Vector2Add(velocity, Vector2Scale(shipVector, 1500));
        //Bullet::Create(shotPos, shotVel, Orientation);

        //Sounds::PlaySoundEffect(Sounds::Shot);
    }
}

void Player::Draw()
{
    if (isAlive)
    {
        if (isThrusting) // draw the thrust flame (below the ship so it doesn't clip over it)
        {
            float extension = boost ? 25.0f : 3.0f;
            float sizeOffset = cosf((float)GetTime() * 20) * extension + (radius * 1.2f);
            Vector2 offset = { 0, -sizeOffset * 0.5f };

            //size_t sprite = boost ? Sprites::TurboThustSprite : Sprites::ThrustSprite;

            //Sprites::Draw(sprite, Position, Orientation, Vector2{ radius * 0.45f, sizeOffset }, WHITE, offset);
        }

        // draw the ship
        DrawNode(shipTexture, position, orientation, { -1.0f, -1.0f }, WHITE, { 0.0f, 0.0f });
        //Sprites::Draw(Sprites::ShipSprite, Position, Orientation);
    }

    // the sheild may need to be drawn after we die since that may be the hit that killed us
    if (shieldHitLifetime > 0)
    {
        BeginBlendMode(BLEND_ADDITIVE);

        float param = shieldHitLifetime / shieldHitMaxLife;
        float invParam = 1.0f - param;
        float size = radius * 3 + (invParam * radius * 3);
        //Color c = ColorAlpha(Sprites::ColorLerp(YELLOW, WHITE, param), param);

        //Sprites::Draw(Sprites::ShieldHitBase, Position, ShieldHitAngle, Vector2{ size,size }, c);

        if (param < 0.3f)
            //Sprites::Draw(Sprites::ShieldHitEnd, Position, ShieldHitAngle, Vector2{ size,size }, c);
        if (param < 0.6f)
            //Sprites::Draw(Sprites::ShieldHitMid, Position, ShieldHitAngle, Vector2{ size,size }, c);

        EndBlendMode();
    }
}

const Vector2& Player::GetPosition() const
{
    return position;
}
