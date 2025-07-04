//
// Created by fredb on 7/4/2025.
//

#pragma once

#include "SDL_render.h"
#include "Actor.h"

class Bee : public Actor
{
public:
    explicit Bee(Game* game, float forwardSpeed = 150.0f, float deathTime = 0.3f, float life = 40.0f);

    void OnUpdate(float deltaTime) override;
    void Kill() override;
    void TakeDamage(float damage);
    void DrawLifeBar(SDL_Renderer* renderer);

    bool IsHealthBarVisible() const { return mHealthBarVisibleTimer > 0.0f; }
    float GetCurrentLife() const { return mCurrentLife; }
    float GetMaxLife() const { return mMaxLife; }

private:
    float mForwardSpeed;
    float mMaxLife;
    float mCurrentLife;
    float mDyingTimer;
    float mHealthBarVisibleTimer;
    float mSineTime;

    bool mIsDying;

    class RigidBodyComponent* mRigidBodyComponent;
    class DrawAnimatedComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;

    void UpdateSineMovement(float deltaTime);
    float kHealthBarDisplayTime = 2.0f;
};