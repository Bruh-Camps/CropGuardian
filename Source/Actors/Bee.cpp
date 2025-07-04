//
// Created by fredb on 7/4/2025.
//

#include "Bee.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Random.h"

Bee::Bee(Game* game, float forwardSpeed, float deathTime, float life)
    : Actor(game)
    , mIsDying(false)
    , mDyingTimer(deathTime)
    , mMaxLife(life)
    , mCurrentLife(life)
    , mHealthBarVisibleTimer(0.0f)
    , mForwardSpeed(forwardSpeed)
    , mCurrentMovementDirection(MovementDirection::Right)
    , mOscillationTimer(0.0f)
    , mOscillationFrequency(5.0f) // Quão rápido a abelha oscila
    , mOscillationAmplitude(60.0f) // Quão longe ela se move perpendicularmente
{
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f);

    // mColliderComponent = new AABBColliderComponent(this, 0, 0,
    //                                                Game::TILE_SIZE, Game::TILE_SIZE,
    //                                                ColliderLayer::Enemy);

    mDrawComponent = new DrawAnimatedComponent(this,
                                               "../Assets/Sprites/Bee/bee.png",
                                               "../Assets/Sprites/Bee/bee.json",
                                               10);

    mDrawComponent->AddAnimation("dead", {1, 2, 3, 4, 5});
    mDrawComponent->AddAnimation("fly", {6, 7, 8, 9, 10, 11});

    mDrawComponent->SetAnimation("fly");
    mDrawComponent->SetAnimFPS(12.0f);
}

void Bee::Kill()
{
    mIsDying = true;
    mDrawComponent->SetAnimation("dead");
    mDrawComponent->SetAnimFPS(10.0f);
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);
    mGame->GetCurrentBase()->IncreaseCoinsBy(5);
}

void Bee::OnUpdate(float deltaTime)
{
    if (mIsDying)
    {
        mDyingTimer -= deltaTime;
        if (mDyingTimer <= 0.0f) {
            mState = ActorState::Destroy;
        }
        return;
    }

    if (mHealthBarVisibleTimer > 0.0f)
    {
        mHealthBarVisibleTimer -= deltaTime;
    }

    if (GetPosition().y > GetGame()->GetWindowHeight())
    {
        mState = ActorState::Destroy;
    }

    UpdateDirectionFromCurrentBlock();
    ApplyBeeMovement(deltaTime);
}

void Bee::ApplyBeeMovement(float deltaTime)
{
    mOscillationTimer += deltaTime;

    Vector2 pathVelocity;
    Vector2 oscillation;

    float oscillationValue = sin(mOscillationTimer * mOscillationFrequency) * mOscillationAmplitude;

    switch (mCurrentMovementDirection)
    {
        case MovementDirection::Right:
            pathVelocity = Vector2::UnitX * mForwardSpeed;
            oscillation.y = oscillationValue;
            break;
        case MovementDirection::Left:
            pathVelocity = Vector2::NegUnitX * mForwardSpeed;
            oscillation.y = oscillationValue;
            break;
        case MovementDirection::Up:
            pathVelocity = Vector2::NegUnitY * mForwardSpeed;
            oscillation.x = oscillationValue;
            break;
        case MovementDirection::Down:
            pathVelocity = Vector2::UnitY * mForwardSpeed;
            oscillation.x = oscillationValue;
            break;
    }

    mRigidBodyComponent->SetVelocity(pathVelocity + oscillation);
}

void Bee::UpdateDirectionFromCurrentBlock()
{
    const int tileSize = Game::TILE_SIZE;
    Vector2 position = this->GetPosition();

    int tileX = static_cast<int>(position.x) / tileSize;
    int tileY = static_cast<int>(position.y) / tileSize;

    float offsetX = static_cast<float>(tileX) * tileSize + tileSize / 2.0f;
    float offsetY = static_cast<float>(tileY) * tileSize + tileSize / 2.0f;

    const float triggerOffset = 16.0f;
    offsetX += triggerOffset;
    offsetY += triggerOffset;


    bool passedMiddle = false;
    switch (mCurrentMovementDirection)
    {
        case MovementDirection::Right: passedMiddle = position.x >= offsetX; break;
        case MovementDirection::Left:  passedMiddle = position.x <= offsetX; break;
        case MovementDirection::Up:    passedMiddle = position.y <= offsetY; break;
        case MovementDirection::Down:  passedMiddle = position.y >= offsetY; break;
    }

    if (!passedMiddle)
        return;

    int tileID = mGame->GetTileAt(tileX, tileY);

    switch (tileID)
    {
        case 12: // Canto inferior direito
            mCurrentMovementDirection = (mCurrentMovementDirection == MovementDirection::Right) ? MovementDirection::Up : MovementDirection::Left;
            break;
        case 10: // Canto superior direito
            mCurrentMovementDirection = (mCurrentMovementDirection == MovementDirection::Right) ? MovementDirection::Down : MovementDirection::Left;
            break;
        case 2:  // Canto inferior esquerdo
            mCurrentMovementDirection = (mCurrentMovementDirection == MovementDirection::Left) ? MovementDirection::Up : MovementDirection::Right;
            break;
        case 17: // Canto superior esquerdo
            mCurrentMovementDirection = (mCurrentMovementDirection == MovementDirection::Left) ? MovementDirection::Down : MovementDirection::Right;
            break;
    }
}


void Bee::TakeDamage(float damage)
{
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

void Bee::DrawLifeBar(SDL_Renderer* renderer)
{
    if (this->IsHealthBarVisible())
    {
        Vector2 cameraPos = GetGame()->GetCameraPos();
        const int barWidth = 40;
        const int barHeight = 5;
        const Vector2 barOffset = Vector2(15.0f, -15.0f);

        Vector2 barPos;
        barPos.x = mPosition.x + barOffset.x - cameraPos.x;
        barPos.y = mPosition.y + barOffset.y - cameraPos.y;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_Rect bgRect = {
            static_cast<int>(barPos.x - barWidth / 2.0f),
            static_cast<int>(barPos.y - barHeight / 2.0f),
            barWidth,
            barHeight
        };
        SDL_RenderFillRect(renderer, &bgRect);

        float lifePercent = mCurrentLife / mMaxLife;
        int fgWidth = static_cast<int>(barWidth * lifePercent);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect fgRect = {
            static_cast<int>(barPos.x - barWidth / 2.0f),
            static_cast<int>(barPos.y - barHeight / 2.0f),
            fgWidth,
            barHeight
        };
        SDL_RenderFillRect(renderer, &fgRect);
    }
}
