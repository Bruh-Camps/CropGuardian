#pragma once

#include "../Actor.h"
#include "../Enemy/Enemy.h"

class Tower : public Actor {
public:
    explicit Tower(Game *game,
                   float damage,
                   float coolDown,
                   float range,
                   const std::string &spriteSheet,
                   const std::string &spriteJson);

    void OnUpdate(float deltaTime) override;

    virtual void ShotProjectile(Vector2 initialPosition, float initialAngle, Enemy* currentTarget) {};

private:
    class DrawAnimatedComponent *mDrawComponent;
    float mDamage;
    float mCoolDown;
    float mCurrentCooldown;
    float mRange;
    Enemy* mCurrentTarget;
};
