//
// Created by fredb on 6/11/2025.
//

#include "BuildSpot.h"

#include "NormalTower.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../../Source/BuildTowerHUD.h"
#include "../Game.h"

BuildSpot::BuildSpot(Game* game, Vector2 dims)
: Actor(game)
, mState(State::Empty)
, mTower(nullptr)
, mIsHovered(false)
{
    mBuildSpotW = dims.x;
    mBuildSpotH = dims.y;

    mSprite = new DrawSpriteComponent(
        this,
        "../Assets/Sprites/Tower/build_spot.png",
        mBuildSpotW,
        mBuildSpotH,
        20);
}

bool BuildSpot::ContainsScreenPoint(int scrX, int scrY)
{
    Vector2 cam = GetGame()->GetCameraPos();
    float x0 = mPosition.x - cam.x;
    float y0 = mPosition.y - cam.y;

    float w = mBuildSpotW  * mScale;
    float h = mBuildSpotH  * mScale;

    return (scrX >= x0 && scrX <= x0 + w &&
            scrY >= y0 && scrY <= y0 + h);
}

void BuildSpot::OnMouseClick()
{
    if (mState == State::Empty)
        GetGame()->GetBuildTowerHUD()->Show(this);

}

void BuildSpot::OnHoverEnter()
{
    if (!mIsHovered)
    {
        mIsHovered = true;
        mSprite->SetScale(1.2f);
    }
}

void BuildSpot::OnHoverExit()
{
    if (mIsHovered)
    {
        mIsHovered = false;
        mSprite->SetScale(1.0f);
    }
}

void BuildSpot::BuildTower(TowerType type) {

    if (type == TowerType::Normal) {
        mTower = new NormalTower(mGame);
        mTower->SetPosition(mPosition - Vector2(0, 64));
        this->SetState(ActorState::Destroy);
        SDL_Log("Tower built!");
    } else {
        SDL_Log("Invalid Tower type!");
    }
}