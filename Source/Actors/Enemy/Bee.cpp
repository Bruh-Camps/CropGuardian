//
// Created by fredb on 7/4/2025.
//

#include "Bee.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../Components/RigidBodyComponent.h"

Bee::Bee(Game* game, float forwardSpeed, float deathTime, float life)
    : Enemy(game, forwardSpeed, deathTime, life, 12,
            "../Assets/Sprites/Bee/bee.png",
            "../Assets/Sprites/Bee/bee.json")
    , mOscillationTimer(0.0f)
    , mOscillationFrequency(10.0f)
    , mOscillationAmplitude(15.0f)
{
    mDrawComponent->AddAnimation("dead", {1, 2, 3, 4, 5});
    mDrawComponent->AddAnimation("fly", {6, 7, 8, 9, 10, 11});

    mDrawComponent->SetAnimation("fly");
    mDrawComponent->SetAnimFPS(12.0f);
}

void Bee::UpdateMovement(float deltaTime)
{
    UpdateDirectionFromCurrentBlock();

    switch (mCurrentMovementDirection)
    {
        case MovementDirection::Right:
            mRigidBodyComponent->SetVelocity(Vector2::UnitX * mForwardSpeed);
            SetRotation(270.0f);
            break;
        case MovementDirection::Left:
            mRigidBodyComponent->SetVelocity(Vector2::NegUnitX * mForwardSpeed);
            SetRotation(90.0f);
            break;
        case MovementDirection::Up:
            mRigidBodyComponent->SetVelocity(Vector2::NegUnitY * mForwardSpeed);
            SetRotation(180.0f);
            break;
        case MovementDirection::Down:
            mRigidBodyComponent->SetVelocity(Vector2::UnitY * mForwardSpeed);
            SetRotation(0.0f);
            break;
    }
}