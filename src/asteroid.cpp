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
    if (!isAlive)
    {
        return;
    }
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
    if (Collide(player))
    {
        player.OnHit();
    }
}

void Asteroid::Draw()
{
    if (!isAlive)
    {
        return;
    }
    Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
    Rectangle dest = { position.x, position.y, (float)texture.width * scale, (float)texture.height * scale };
    Vector2 center = { dest.width / 2.0f, dest.height / 2.0f };
    DrawTexturePro(texture, source, dest, center, orientation, WHITE);
}

bool Asteroid::Collide(Node& other)
{
    auto hit = Node::Collide(other);
    if (hit)
    {
        Explosion::Create(position, 2);
        isAlive = false;
    }
    return hit;
}
