#include "Slime.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Random.h"

#include "../Json.h"

Slime::Slime(Game* game, float forwardSpeed, float deathTime, float life)
        : Actor(game)
        , mDyingTimer(deathTime)
        , mIsDying(false)
        , mForwardSpeed(forwardSpeed)
        , mMaxLife(life)
        , mCurrentLife(life)
        , mHealthBarVisibleTimer(0.0f)
        , mCurrentMovementDirection(MovementDirection::Right)
{
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f);
    mRigidBodyComponent->SetVelocity(Vector2(mForwardSpeed, 0.0f));

    //mColliderComponent = new AABBColliderComponent(this, 0, 0,
    //                                               Game::TILE_SIZE, Game::TILE_SIZE,
    //                                               ColliderLayer::Enemy);

    mDrawComponent = new DrawAnimatedComponent(this,
                                                  "../Assets/Sprites/Slime/slime.png",
                                                  "../Assets/Sprites/Slime/slime.json",
                                                  10);

    mDrawComponent->AddAnimation("dead", {1, 2, 3, 4, 5});
    mDrawComponent->AddAnimation("idle", {0});
    mDrawComponent->AddAnimation("walk", {6, 7, 8, 9, 10, 11});

    mDrawComponent->SetAnimation("walk");
    mDrawComponent->SetAnimFPS(10.0f);
}

void Slime::Kill()
{
    mIsDying = true;
    mDrawComponent->SetAnimation("dead");
    mDrawComponent->SetAnimFPS(5.0f);
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);
    mGame->GetCurrentBase()->IncreaseCoinsBy(7);
}

void Slime::OnUpdate(float deltaTime)
{
    UpdateMovementDirection();
    UpdateDirectionFromCurrentBlock();

    if (mIsDying)
    {
        mDyingTimer -= deltaTime;
        if (mDyingTimer <= 0.0f) {
            mState = ActorState::Destroy;
        }
    }
    else
    {
        if (mHealthBarVisibleTimer > 0.0f)
        {
            mHealthBarVisibleTimer -= deltaTime;
        }
    }

    if (GetPosition().y > GetGame()->GetWindowHeight())
    {
        mState = ActorState::Destroy;
    }
}

void Slime::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    this->OnCollision(other);
}
void Slime::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    this->OnCollision(other);
}


void Slime::OnCollision(AABBColliderComponent* other) {

}

void Slime::UpdateMovementDirection()
{
    Vector2 velocity = mRigidBodyComponent->GetVelocity();

    if (velocity.LengthSq() < 0.01f)
        return;

    if (std::abs(velocity.x) > std::abs(velocity.y))
    {
        mCurrentMovementDirection = (velocity.x > 0) ? MovementDirection::Right : MovementDirection::Left;
    }
    else
    {
        mCurrentMovementDirection = (velocity.y > 0) ? MovementDirection::Down : MovementDirection::Up;
    }
}

void Slime::UpdateDirectionFromCurrentBlock()
{
    const int tileSize = Game::TILE_SIZE;
    Vector2 position = this->GetPosition();

    int tileX = static_cast<int>(position.x) / tileSize;
    int tileY = static_cast<int>(position.y) / tileSize;

    float offsetX = static_cast<float>(tileX) * tileSize + tileSize / 2.0f + 14.0f;
    float offsetY = static_cast<float>(tileY) * tileSize + tileSize / 2.0f + 14.0f;

    // Garante que só muda direção se estiver no meio do tile (ou passou)
    bool passedMiddle = false;

    switch (mCurrentMovementDirection)
    {
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

    // Obtenha o tile ID a partir do mapa
    int tileID = mGame->GetTileAt(tileX, tileY);

    // Lógica de mudança de direção
    switch (tileID)
    {
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

void Slime::TakeDamage(float damage)
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

// Essa função é chamada pelo draw animated component
void Slime::DrawLifeBar(SDL_Renderer* renderer) {
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
        SDL_Rect bgRect;
        bgRect.x = static_cast<int>(barPos.x - barWidth / 2.0f);
        bgRect.y = static_cast<int>(barPos.y - barHeight / 2.0f);
        bgRect.w = barWidth;
        bgRect.h = barHeight;
        SDL_RenderFillRect(renderer, &bgRect);

        float lifePercent = mCurrentLife / mMaxLife;
        int fgWidth = static_cast<int>(barWidth * lifePercent);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect fgRect;
        fgRect.x = static_cast<int>(barPos.x - barWidth / 2.0f);
        fgRect.y = static_cast<int>(barPos.y - barHeight / 2.0f);
        fgRect.w = fgWidth;
        fgRect.h = barHeight;
        SDL_RenderFillRect(renderer, &fgRect);
    }
}
