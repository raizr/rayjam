#include "node.h"
#include "core.h"
#include "raymath.h"


void Node::Update()
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
}

bool Node::Collide(const Node& other)
{
    return CheckCollisionCircles(position, radius, other.position, other.radius);
}
