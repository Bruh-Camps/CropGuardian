//
// Created by Lucas N. Ferreira on 12/09/23.
//

#pragma once
#include "../Actor.h"

class NormalArrow : public Actor
{
public:
    NormalArrow(class Game* game, Actor* target, float force = 300.0f, float damage = 10.0f, float deathTimer = 1.0f);

    void OnUpdate(float deltaTime) override;

private:
    float mMoveForce;
    float mDamage;
    float mDeathTimer;

    class RigidBodyComponent* mRigidBodyComponent;
    class DrawSpriteComponent* mSprite;

    Actor* mCurrentTarget;
};
