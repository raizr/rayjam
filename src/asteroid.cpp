#include "asteroid.h"

#include "core.h"
#include "pool.h"
#include "raymath.h"
#include "resource.h"
#include "scene_manager.h"


Asteroid::Asteroid()
{
    radius = 10.0f;
    texture = Resources::asteroid;
}

void Asteroid::Update()
{
    position = Vector2Add(position, Vector2Scale(velocity, core::Core::getInstance()->GetDeltaTime()));

    // rotate
    orientation += rotationalVelocity * core::Core::getInstance()->GetDeltaTime();

    while (orientation > 180)
    {
        orientation -= 360;
    }
    while (orientation < -180)
    {
        orientation += 360;
    }
    auto& player = scene::SceneManager::getInstance()->GetPlayer();
    CollideWithShip(player);
    if (velocity.x != 0.0f)
    {
        if (position.x >= boundary.y
            || position.x <= boundary.x)
        {
            velocity.x = -velocity.x;
        }
    }
    if (velocity.y != 0.0f)
    {
        if (position.y >= boundary.y
            || position.y <= boundary.x)
        {
            velocity.y = -velocity.y;
        }
    }
}

void Asteroid::CollideWithShip(Node& ship)
{
    if (Collide(ship))
    {
        float dx = ship.position.x - position.x;
        float dy = ship.position.y - position.y;
        ship.velocity.x = dx;
        ship.velocity.y = dy;
    }
}

void Asteroid::Draw()
{
    
    /*DrawTextureEx(texture,
        position,
        orientation, scale, WHITE);*/
    Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
    Rectangle dest = { position.x, position.y, (float)texture.width * scale, (float)texture.height * scale };
    Vector2 center = { dest.width / 2.0f, dest.height / 2.0f };
    DrawTexturePro(texture, source, dest, center, orientation, WHITE);
}
