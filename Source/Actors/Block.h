//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once

#include "Actor.h"
#include <string>

class Block : public Actor
{
public:
    explicit Block(Game* game, const std::string &texturePath, int width, int height, const bool isStatic = true, int drawOrder = 1);

    void SetPosition(const Vector2& position)
    {
        Actor::SetPosition(position);
        mOriginalPosition.Set(position.x, position.y);
    }

    void OnUpdate(float deltaTime) override;
    void OnBump();
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void SetColliderEnabled(bool enabled);

private:
    const int BUMP_FORCE = 200;

    Vector2 mOriginalPosition;

    class AABBColliderComponent* mColliderComponent;
    class RigidBodyComponent* mRigidBodyComponent;
};
