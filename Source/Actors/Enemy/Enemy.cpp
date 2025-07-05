//
// Created by fredb on 7/4/2025.
//

#include "Enemy.h"
#include "../../Game.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../Components/RigidBodyComponent.h"
#include "../../Components/ColliderComponents/AABBColliderComponent.h"

Enemy::Enemy(Game *game, float forwardSpeed, float deathTime, float life, int coinReward,
             const std::string &spriteSheet, const std::string &spriteJson)
    : Actor(game)
      , mIsDying(false)
      , mDyingTimer(deathTime)
      , mMaxLife(life)
      , mCurrentLife(life)
      , mHealthBarVisibleTimer(0.0f)
      , mForwardSpeed(forwardSpeed)
      , mCoinReward(coinReward)
      , mCurrentMovementDirection(MovementDirection::Right)
      , mOriginalSpeed(forwardSpeed)
      , mSlowdownTimer(0.0f)
      , mIsSlowed(false) {
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f);
    mDrawComponent = new DrawAnimatedComponent(this, spriteSheet, spriteJson);

    // mColliderComponent = new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE, Game::TILE_SIZE, ColliderLayer::Enemy);

    mGame->IncrementEnemyCount();
}

void Enemy::OnUpdate(float deltaTime) {
    if (mIsDying) {
        mDyingTimer -= deltaTime;
        if (mDyingTimer <= 0.0f) {
            mState = ActorState::Destroy;
        }
        return;
    }

    if (mHealthBarVisibleTimer > 0.0f) {
        mHealthBarVisibleTimer -= deltaTime;
    }

    if (GetPosition().y > GetGame()->GetWindowHeight()) {
        mState = ActorState::Destroy;
    }

    UpdateMovement(deltaTime);

    // Atualiza o temporizador de lentidão
    if (mIsSlowed)
    {
        mSlowdownTimer -= deltaTime;
        if (mSlowdownTimer <= 0.0f)
        {
            mForwardSpeed = mOriginalSpeed;
            mIsSlowed = false;
            mSlowdownTimer = 0.0f;
        }
    }

}

void Enemy::TakeDamage(float damage) {
    if (mIsDying) {
        return;
    }

    mCurrentLife -= damage;
    mHealthBarVisibleTimer = kHealthBarDisplayTime;

    if (mCurrentLife <= 0.0f) {
        mCurrentLife = 0.0f;
        Kill();
    }
}

void Enemy::Kill() {
    if (!mIsDying) {
        mGame->DecrementEnemyCount();
    }
    mIsDying = true;
    mDrawComponent->SetAnimation("dead");
    //mRigidBodyComponent->SetEnabled(false); // cuidado! ativar causa segfault
    //if (mColliderComponent) mColliderComponent->SetEnabled(false); // cuidado! ativar causa segfault
    this->mRigidBodyComponent->SetVelocity(Vector2(0.0f, 0.0f));
    mGame->GetCurrentBase()->IncreaseCoinsBy(mCoinReward);
}

void Enemy::UpdateDirectionFromCurrentBlock() {
    const int tileSize = Game::TILE_SIZE;
    Vector2 position = this->GetPosition();

    int tileX = static_cast<int>(position.x) / tileSize;
    int tileY = static_cast<int>(position.y) / tileSize;

    float offsetX = static_cast<float>(tileX) * tileSize + tileSize / 2.0f + 14.0f;
    float offsetY = static_cast<float>(tileY) * tileSize + tileSize / 2.0f + 14.0f;

    bool passedMiddle = false;
    switch (mCurrentMovementDirection) {
        case MovementDirection::Right:
            passedMiddle = position.x >= offsetX;
            break;
        case MovementDirection::Left:
            passedMiddle = position.x <= offsetX;
            break;
        case MovementDirection::Up:
            passedMiddle = position.y <= offsetY;
            break;
        case MovementDirection::Down:
            passedMiddle = position.y >= offsetY;
            break;
    }

    if (!passedMiddle)
        return;

    int tileID = mGame->GetTileAt(tileX, tileY);

    switch (tileID) {
        case 12: // Canto inferior direito
            if (mCurrentMovementDirection == MovementDirection::Right)
                mCurrentMovementDirection = MovementDirection::Up;
            else if (mCurrentMovementDirection == MovementDirection::Down)
                mCurrentMovementDirection = MovementDirection::Left;
            break;

        case 10: // Canto superior direito
            if (mCurrentMovementDirection == MovementDirection::Right)
                mCurrentMovementDirection = MovementDirection::Down;
            else if (mCurrentMovementDirection == MovementDirection::Up)
                mCurrentMovementDirection = MovementDirection::Left;
            break;

        case 2: // Canto inferior esquerdo
            if (mCurrentMovementDirection == MovementDirection::Left)
                mCurrentMovementDirection = MovementDirection::Up;
            else if (mCurrentMovementDirection == MovementDirection::Down)
                mCurrentMovementDirection = MovementDirection::Right;
            break;

        case 17: // Canto superior esquerdo
            if (mCurrentMovementDirection == MovementDirection::Left)
                mCurrentMovementDirection = MovementDirection::Down;
            else if (mCurrentMovementDirection == MovementDirection::Up)
                mCurrentMovementDirection = MovementDirection::Right;
            break;
    }
}

void Enemy::DrawLifeBar(SDL_Renderer *renderer) {
    if (this->IsHealthBarVisible()) {
        Vector2 cameraPos = GetGame()->GetCameraPos();
        const int barWidth = 40;
        const int barHeight = 5;
        const Vector2 barOffset = Vector2(15.0f, -15.0f);

        Vector2 barPos;
        barPos.x = mPosition.x + barOffset.x - cameraPos.x;
        barPos.y = mPosition.y + barOffset.y - cameraPos.y;

        SDL_Rect bgRect = {
            static_cast<int>(barPos.x - barWidth / 2.0f), static_cast<int>(barPos.y - barHeight / 2.0f), barWidth,
            barHeight
        };
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &bgRect);

        float lifePercent = mCurrentLife / mMaxLife;
        int fgWidth = static_cast<int>(barWidth * lifePercent);

        SDL_Rect fgRect = {
            static_cast<int>(barPos.x - barWidth / 2.0f), static_cast<int>(barPos.y - barHeight / 2.0f), fgWidth,
            barHeight
        };
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &fgRect);
    }
}

void Enemy::SlowDown(float percentage, float duration)
{
    // Evita aplicar múltiplas lentidões simultaneamente
    if (!mIsSlowed)
    {
        mOriginalSpeed = mForwardSpeed;
        mForwardSpeed *= percentage;
        mSlowdownTimer = duration;
        mIsSlowed = true;
    }
    else
    {
        // Reinicia o tempo da lentidão se já estiver lento
        mSlowdownTimer = duration;
    }
}

