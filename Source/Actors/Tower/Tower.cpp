#include "Tower.h"

#include "../Enemy/Slime.h"
#include "../../Game.h"
#include "../../MainHUD.h"
#include "Projectile.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"

Tower::Tower(Game *game,
             float damage,
             float coolDown,
             float range,
             const std::string &spriteSheet,
             const std::string &spriteJson)
    : Actor(game)
      , mDamage(damage)
      , mCoolDown(coolDown)
      , mRange(range)
      , mCurrentTarget(nullptr) {
    mDrawComponent = new DrawAnimatedComponent(this,
                                               spriteSheet,
                                               spriteJson,
                                               20);

    mDrawComponent->AddAnimation("Active", {0, 1, 2, 3, 4, 5});
    mDrawComponent->SetAnimation("Active");
    mDrawComponent->SetAnimFPS(10.0f);

    mCurrentCooldown = mCoolDown;
}

void Tower::OnUpdate(float deltaTime) {
    mCurrentCooldown -= deltaTime;

    // Deve-se considerar a posição central do sprite
    Vector2 correctedPos = GetPosition() + Vector2(32, 72);

    // Verifica se tem um alvo atual e se ainda está dentro do alcance
    if (mCurrentTarget) {
        Vector2 targetPos = mCurrentTarget->GetPosition();
        float distance = (targetPos - correctedPos).Length();

        if (mCurrentTarget->GetState() == ActorState::Destroy || distance > mRange) {
            mCurrentTarget = nullptr; // Perdeu o alvo
        }
    }

    // Se não há alvo, procura o mais próximo dentro do alcance
    if (!mCurrentTarget) {
        float closestDistance = mRange;
        std::vector<Actor *> nearbyActors = mGame->GetNearbyActors(correctedPos, 3);

        for (Actor *actor: nearbyActors) {
            if (auto enemy = dynamic_cast<Enemy *>(actor)) {
                float distance = (enemy->GetPosition() - correctedPos).Length();

                if (distance <= closestDistance) {
                    closestDistance = distance;
                    mCurrentTarget = enemy;
                }
            }
        }
    }

    if (mCurrentTarget) {
        Vector2 targetPos = mCurrentTarget->GetPosition() + Vector2(16, 16);
        Vector2 direction = Vector2::Normalize(targetPos - correctedPos);

        // A torre acompanha o inimigo com a rotação sempre
        float angleRad = atan2f(direction.y, direction.x);
        float angleDeg = angleRad * (180.0f / Math::Pi);

        //SetRotation(angleDeg);

        // Se o cooldown acabou, atira
        if (mCurrentCooldown <= 0.0f) {
            ShotProjectile(correctedPos, angleDeg, mCurrentTarget);
            mCurrentCooldown = mCoolDown;
        }
    }
}

