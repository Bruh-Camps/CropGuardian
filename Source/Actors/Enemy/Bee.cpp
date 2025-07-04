//
// Created by fredb on 7/4/2025.
//

#include "Bee.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../Components/RigidBodyComponent.h"

Bee::Bee(Game* game, float forwardSpeed, float deathTime, float life)
    : Enemy(game, forwardSpeed, deathTime, life, 5,
            "../Assets/Sprites/Bee/bee.png",
            "../Assets/Sprites/Bee/bee.json")
    , mOscillationTimer(0.0f)
    , mOscillationFrequency(5.0f)
    , mOscillationAmplitude(60.0f)
{
    mDrawComponent->AddAnimation("dead", {1, 2, 3, 4, 5});
    mDrawComponent->AddAnimation("fly", {6, 7, 8, 9, 10, 11});

    mDrawComponent->SetAnimation("fly");
    mDrawComponent->SetAnimFPS(12.0f);
}

void Bee::UpdateMovement(float deltaTime)
{
    UpdateDirectionFromCurrentBlock();
    ApplyOscillationMovement(deltaTime);
}

void Bee::ApplyOscillationMovement(float deltaTime)
{
    mOscillationTimer += deltaTime;

    Vector2 pathVelocity;
    Vector2 oscillation;

    float oscillationValue = sin(mOscillationTimer * mOscillationFrequency) * mOscillationAmplitude;

    switch (mCurrentMovementDirection)
    {
        case MovementDirection::Right: pathVelocity = Vector2::UnitX * mForwardSpeed; oscillation.y = oscillationValue; break;
        case MovementDirection::Left:  pathVelocity = Vector2::NegUnitX * mForwardSpeed; oscillation.y = oscillationValue; break;
        case MovementDirection::Up:    pathVelocity = Vector2::NegUnitY * mForwardSpeed; oscillation.x = oscillationValue; break;
        case MovementDirection::Down:  pathVelocity = Vector2::UnitY * mForwardSpeed; oscillation.x = oscillationValue; break;
    }

    mRigidBodyComponent->SetVelocity(pathVelocity + oscillation);
}