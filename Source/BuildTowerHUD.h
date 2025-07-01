//
// Created by fredb on 6/11/2025.
//

#pragma once

#include "UIElements/UIScreen.h"

class UIImageButton;
class UIImage;
class BuildSpot;

class BuildTowerHUD : public UIScreen
{
public:
    BuildTowerHUD(class Game* game, const std::string& fontName, SDL_Renderer* renderer);

    void Show(BuildSpot* activeSpot);

    void Hide();

    void Draw(SDL_Renderer* renderer) override;

    bool isVisible() const {return mIsVisible;}

private:
    BuildSpot* mActiveBuildSpot;

    UIImage* mBackgroundPanel;
    UIImageButton* mTower1;
    UIImageButton* mTower2;
    UIImageButton* mTower3;

    bool mIsVisible;
};
