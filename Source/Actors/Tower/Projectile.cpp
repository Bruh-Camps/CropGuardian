//
// Created by Lucas N. Ferreira on 12/09/23.
//

#include "Projectile.h"
#include "../Enemy/Enemy.h"
#include "../../Game.h"
#include "../../Components/RigidBodyComponent.h"
#include "../../Components/DrawComponents/DrawSpriteComponent.h"

Projectile::Projectile(Game* game, Actor* target, ProjectileType type, float force, float damage, const float deathTimer)
        :Actor(game)
        ,mType(type)
        ,mMoveForce(force)
        ,mDamage(damage)
        ,mDeathTimer(deathTimer)
        ,mCurrentTarget(target)
{

    if (mType == ProjectileType::Nornal) {
        mSprite = new DrawSpriteComponent(
        this,
        "../Assets/Sprites/NormalTower/arrow.png",
        3,
        14,
        20);
    } else if (mType == ProjectileType::Ice) {
        mSprite = new DrawSpriteComponent(
        this,
        "../Assets/Sprites/IceTower/ice_shot.png",
        20,
        20,
        20);
    }



    mRigidBodyComponent = new RigidBodyComponent(this, 1);
    mRigidBodyComponent->SetApplyGravity(false);

    Vector2 targetPos = mCurrentTarget->GetPosition() + Vector2(16,16);
    Vector2 direction = Vector2::Normalize(targetPos - GetPosition());

    // Atualiza a rotação inicial
    float angle = atan2f(direction.y, direction.x) * (180.0f / Math::Pi);
    SetRotation(angle + 90.0f);

}

void Projectile::OnUpdate(float deltaTime)
{
    mDeathTimer -= deltaTime;
    if (mDeathTimer <= 0.0f || !mCurrentTarget || mCurrentTarget->GetState() == ActorState::Destroy) {
        SetState(ActorState::Destroy);
        return;
    }

    Vector2 myPos = GetPosition();
    Vector2 targetPos = mCurrentTarget->GetPosition() + Vector2(16,16);

    // Calcula direção atualizada
    Vector2 direction = Vector2::Normalize(targetPos - myPos);

    // Atualiza velocidade diretamente para seguir o alvo
    mRigidBodyComponent->SetVelocity(direction * mMoveForce);

    // Atualiza rotação
    float angle = atan2f(direction.y, direction.x) * (180.0f / Math::Pi);
    SetRotation(angle + 90.0f);

    // Verifica colisão
    float distance = (targetPos - myPos).Length();
    if (distance < 10.0f) {
        if (auto enemy = dynamic_cast<Enemy*>(mCurrentTarget)) {
            enemy->TakeDamage(mDamage);
            if (mType==ProjectileType::Ice) enemy->SlowDown(0.6f, 3.0f);  // Reduz velocidade para 60% por 3 segundos
        }
        SetState(ActorState::Destroy);
    }
}
