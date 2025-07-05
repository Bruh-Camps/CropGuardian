//
// Created by Lucas N. Ferreira on 12/09/23.
//

#include "NormalArrow.h"
#include "../Enemy/Enemy.h"
#include "../../Game.h"
#include "../../Components/RigidBodyComponent.h"
#include "../../Components/DrawComponents/DrawSpriteComponent.h"

NormalArrow::NormalArrow(Game* game, Actor* target, float force, float damage, const float deathTimer)
        :Actor(game)
        ,mMoveForce(force)
        ,mDamage(damage)
        ,mDeathTimer(deathTimer)
        ,mCurrentTarget(target)
{

    mSprite = new DrawSpriteComponent(
        this,
        "../Assets/Sprites/Tower/arrow.png",
        3,
        14,
        20);


    mRigidBodyComponent = new RigidBodyComponent(this, 1);
    mRigidBodyComponent->SetApplyGravity(false);

    Vector2 targetPos = mCurrentTarget->GetPosition() + Vector2(16,16);
    Vector2 direction = Vector2::Normalize(targetPos - GetPosition());

    // Atualiza a rotação inicial
    float angle = atan2f(direction.y, direction.x) * (180.0f / Math::Pi);
    SetRotation(angle + 90.0f);

}

void NormalArrow::OnUpdate(float deltaTime)
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
        }
        SetState(ActorState::Destroy);
    }
}
