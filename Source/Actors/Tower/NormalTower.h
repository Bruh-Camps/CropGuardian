#pragma once

#include "Tower.h"
#include "../Actor.h"

class NormalTower : public Tower {
public:
    explicit NormalTower(Game *game);


private:
    float mProjectileMoveForce;
    float mProjectileDamage;
    void ShotProjectile(Vector2 initialPosition, float initialAngle, Enemy *currentTarget) override;


};
