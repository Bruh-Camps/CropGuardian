#include "IceTower.h"

#include "../Enemy/Slime.h"
#include "../../Game.h"
#include "../../MainHUD.h"
#include "Projectile.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"

IceTower::IceTower(Game *game)
    : mProjectileMoveForce(240)
      , mProjectileDamage(5.0f)
      , Tower(game, 2.0f
              , 0.5f
              , 100.0f
              , "../Assets/Sprites/IceTower/ice_tower.png"
              , "../Assets/Sprites/IceTower/ice_tower.json") {
}

void IceTower::ShotProjectile(Vector2 initialPosition, float initialAngle, Enemy *currentTarget) {
    auto *arrow = new Projectile(this->GetGame(), currentTarget, Projectile::ProjectileType::Ice,
                                 mProjectileMoveForce, mProjectileDamage, 2.0f);
    arrow->SetPosition(initialPosition);
    arrow->SetRotation(initialAngle + 90);
}
