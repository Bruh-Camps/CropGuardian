#include "Slime.h"
#include "../../Json.h"
#include "../../Game.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../Components/DrawComponents/DrawPolygonComponent.h"
#include "../../Components/RigidBodyComponent.h"

Slime::Slime(Game* game, float forwardSpeed, float deathTime, float life)
    : Enemy(game, forwardSpeed, deathTime, life, 7,
            "../Assets/Sprites/Slime/slime.png",
            "../Assets/Sprites/Slime/slime.json")
{
    mDrawComponent->AddAnimation("dead", {1, 2, 3, 4, 5});
    mDrawComponent->AddAnimation("idle", {0});
    mDrawComponent->AddAnimation("walk", {6, 7, 8, 9, 10, 11});

    mDrawComponent->SetAnimation("walk");
    mDrawComponent->SetAnimFPS(10.0f);
}

void Slime::UpdateMovement(float deltaTime)
{
    UpdateDirectionFromCurrentBlock();

    switch (mCurrentMovementDirection)
    {
        case MovementDirection::Right:
            mRigidBodyComponent->SetVelocity(Vector2::UnitX * mForwardSpeed);
            break;
        case MovementDirection::Left:
            mRigidBodyComponent->SetVelocity(Vector2::NegUnitX * mForwardSpeed);
            break;
        case MovementDirection::Up:
            mRigidBodyComponent->SetVelocity(Vector2::NegUnitY * mForwardSpeed);
            break;
        case MovementDirection::Down:
            mRigidBodyComponent->SetVelocity(Vector2::UnitY * mForwardSpeed);
            break;
    }
}
