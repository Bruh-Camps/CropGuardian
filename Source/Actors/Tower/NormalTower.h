#pragma once

#include "../Actor.h"
#include "../Enemy/Enemy.h"

class NormalTower : public Actor{
    public:
        explicit NormalTower(Game* game);
        void OnUpdate(float deltaTime) override;

    private:
        class DrawAnimatedComponent* mDrawComponent;
        float mDamage;
        float mCooldown;
        float mCurrentCooldown;
        float mRange;
        float mProjectileMoveForce;
        float mProjectileDamage;
        Enemy* mCurrentTarget;
};
