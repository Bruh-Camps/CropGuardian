#include "Block.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"

Block::Block(Game* game, const std::string &texturePath, int width, int height, const bool isStatic, int drawOrder)
        :Actor(game)
{
    new DrawSpriteComponent(this, texturePath, width, height, drawOrder);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE, Game::TILE_SIZE, ColliderLayer::Blocks, isStatic);
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0.0f, false);

    // Disable collider and rigid body
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);
}

void Block::SetColliderEnabled(bool enabled)
{
    if (mColliderComponent)
    {
        mColliderComponent->SetEnabled(enabled);
    }
}

void Block::OnBump()
{
    if (mPosition.y != mOriginalPosition.y)
    {
        mPosition.Set(mOriginalPosition.x, mOriginalPosition.y);
    }

    // Disable collider
    mColliderComponent->SetStatic(false);
    mRigidBodyComponent->SetVelocity(Vector2::NegUnitY * BUMP_FORCE);
    mRigidBodyComponent->SetApplyGravity(true);
}

void Block::OnUpdate(float deltaTime)
{
    // If the block is moving, check if it has reached its original position
    if (mRigidBodyComponent->GetVelocity().y > 0.0f && mPosition.y >= mOriginalPosition.y)
    {
        mPosition.Set(mOriginalPosition.x, mOriginalPosition.y);
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        mRigidBodyComponent->SetApplyGravity(false);
        mColliderComponent->SetStatic(true);
    }
}

void Block::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    // If collide against enemy, apply bump force
    /*if (other->GetLayer() == ColliderLayer::Enemy)
    {
        Goomba* goomba = static_cast<Goomba*>(other->GetOwner());
        goomba->BumpKill();

        mRigidBodyComponent->SetVelocity(Vector2::NegUnitY * BUMP_FORCE);
        mRigidBodyComponent->SetApplyGravity(true);
        mOriginalPosition.Set(mPosition.x, mPosition.y);
    }*/
}

