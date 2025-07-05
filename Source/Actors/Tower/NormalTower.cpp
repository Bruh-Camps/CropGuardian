#include "NormalTower.h"

#include "Tower.h"
#include "../Enemy/Slime.h"
#include "../../Game.h"
#include "../../MainHUD.h"
#include "Projectile.h"
#include "Tower.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"

NormalTower::NormalTower(Game *game)
    : mProjectileMoveForce(300)
      , mProjectileDamage(10.0f)
      , Tower(game, 2.0f
              , 0.5f
              , 100.0f
              , "../Assets/Sprites/NormalTower/normal_tower.png"
              , "../Assets/Sprites/NormalTower/normal_tower.json") {
}

void NormalTower::ShotProjectile(Vector2 initialPosition, float initialAngle, Enemy *currentTarget) {
    auto *arrow = new Projectile(this->GetGame(), currentTarget, Projectile::ProjectileType::Nornal,
                                 mProjectileMoveForce, mProjectileDamage, 2.0f);
    arrow->SetPosition(initialPosition);
    arrow->SetRotation(initialAngle + 90);
}
