//
// Created by fredb on 6/11/2025.
//

#pragma once
#include "Actor.h"
#include "Tower/NormalTower.h"

constexpr float HOVER_SCALE = 1.12f;

class BuildSpot : public Actor
{
public:
    enum class State {
        Empty,
        Occupied
    };

    enum class TowerType
    {
        Normal,
        Ice,
        Fire
    };

    BuildSpot(Game* game, Vector2 dims);

    void OnMouseClick();
    void OnHoverEnter();
    void OnHoverExit();

    void BuildTower(TowerType type);

    bool ContainsScreenPoint(int scrX, int scrY);

    State GetState() const { return mState; }
    bool IsHovered() const { return mIsHovered; }

private:
    float mBuildSpotW;
    float mBuildSpotH;

    State mState;
    class NormalTower* mTower;
    class DrawSpriteComponent* mSprite;
    bool mIsHovered = false;
};