#pragma once

#include "SDL2/SDL_render.h"
#include "Actor.h"

class Slime : public Actor
{
public:
    enum class MovementDirection
    {
        Up,
        Down,
        Left,
        Right
    };

    explicit Slime(Game* game, float forwardSpeed = 100.0f, float deathTime = 0.5f, float life = 100.0f);

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;

    void Kill() override;
    void BumpKill(const float bumpForce = 300.0f);
    void TakeDamage(float damage);
    void UpdateMovementDirection();
    MovementDirection GetCurrentMovementDirection() const { return mCurrentMovementDirection; }

    void DrawLifeBar(SDL_Renderer* renderer);

    bool IsHealthBarVisible() const { return mHealthBarVisibleTimer > 0.0f; }
    float GetCurrentLife() const { return mCurrentLife; }
    float GetMaxLife() const { return mMaxLife; }

private:
    bool mIsDying;
    float mDyingTimer;

    float mMaxLife;
    float mCurrentLife;
    float mHealthBarVisibleTimer;

    float mForwardSpeed;
    MovementDirection mCurrentMovementDirection;

    class RigidBodyComponent* mRigidBodyComponent;
    class DrawAnimatedComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;

    void OnCollision(AABBColliderComponent* other);
    void UpdateDirectionFromCurrentBlock();

    float kHealthBarDisplayTime = 2.0f;
};