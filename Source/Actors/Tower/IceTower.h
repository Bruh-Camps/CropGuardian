#pragma once

#include "Tower.h"
#include "../Actor.h"

class IceTower : public Tower{
public:
    explicit IceTower(Game* game);

private:
    float mProjectileMoveForce;
    float mProjectileDamage;

    void ShotProjectile(Vector2 initialPosition, float initialAngle, Enemy *currentTarget) override;

};
