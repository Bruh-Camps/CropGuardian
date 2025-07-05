//
// Created by fredb on 7/4/2025.
//

#pragma once

#include "../Actor.h"
#include <string>

class Enemy : public Actor
{
public:
    enum class MovementDirection
    {
        Up,
        Down,
        Left,
        Right
    };

    Enemy(class Game* game, float forwardSpeed, float deathTime, float life, int coinReward,
          const std::string& spriteSheet, const std::string& spriteJson);

    void OnUpdate(float deltaTime) override;

    void TakeDamage(float damage);
    void Kill() override;
    void DrawLifeBar(SDL_Renderer* renderer) override;
    void SlowDown(float percentage, float duration);

    bool IsHealthBarVisible() const { return mHealthBarVisibleTimer > 0.0f; }
    float GetCurrentLife() const { return mCurrentLife; }
    float GetMaxLife() const { return mMaxLife; }

protected:
    virtual void UpdateMovement(float deltaTime) = 0;

    void UpdateDirectionFromCurrentBlock();

    bool mIsDying;
    float mDyingTimer;

    float mMaxLife;
    float mCurrentLife;
    float mHealthBarVisibleTimer;

    float mForwardSpeed;
    int mCoinReward;
    MovementDirection mCurrentMovementDirection;

    class RigidBodyComponent* mRigidBodyComponent;
    class DrawAnimatedComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;

    // Para os efeitos de gelo
    float mOriginalSpeed;         // Armazena a velocidade original
    float mSlowdownTimer;         // Contador para o efeito de lentidão
    bool mIsSlowed;               // Indica se está sob efeito


    const float kHealthBarDisplayTime = 2.0f;
};