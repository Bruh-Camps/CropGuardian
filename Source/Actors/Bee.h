//
// Created by fredb on 7/4/2025.
//

#pragma once

#include "SDL_render.h"
#include "Actor.h"

class Bee : public Actor
{
public:
    enum class MovementDirection
    {
        Up,
        Down,
        Left,
        Right
    };

    explicit Bee(class Game* game, float forwardSpeed = 120.0f, float deathTime = 0.4f, float life = 70.0f);

    void OnUpdate(float deltaTime) override;

    void Kill() override;
    void TakeDamage(float damage);

    void DrawLifeBar(SDL_Renderer* renderer) override;

    bool IsHealthBarVisible() const { return mHealthBarVisibleTimer > 0.0f; }
    float GetCurrentLife() const { return mCurrentLife; }
    float GetMaxLife() const { return mMaxLife; }

private:
    void UpdateDirectionFromCurrentBlock();
    void ApplyBeeMovement(float deltaTime);

    bool mIsDying;
    float mDyingTimer;

    float mMaxLife;
    float mCurrentLife;
    float mHealthBarVisibleTimer;

    float mForwardSpeed;
    MovementDirection mCurrentMovementDirection;

    float mOscillationTimer;
    float mOscillationFrequency;
    float mOscillationAmplitude;

    class RigidBodyComponent* mRigidBodyComponent;
    class DrawAnimatedComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;

    const float kHealthBarDisplayTime = 2.0f;
};