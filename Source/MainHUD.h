//
// Created by Lucas N. Ferreira on 08/12/23.
//

#pragma once

#include <string>

#include "UIElements/UIScreen.h"

class MainHUD : public UIScreen
{
public:

    MainHUD(class Game *game, const std::string &fontName, SDL_Renderer *renderer);

    ~MainHUD();

    void SetLevelName(const std::string& name);
    void SetCoinCount(int count);
    void SetTime(float time);
    void SetWaveCount(int currentWave, int totalWaves);
    void UpdatePauseButtonIcon(bool isPaused);
    void SetLives(int currentLives, int maxLives = -1);

private:
  	UIText* mLevelNameText;
    UIImage* mBannerImage;

    UIImageButton* mPauseButton;

    UIImage* mCoinIcon;
    UIText* mCoinCountText;

    UIImage* mWaveIcon;
    UIText* mWaveCountText;

    class UIText* mTimeText;
    class UIImage* mTimeIcon;

    std::vector<class UIImage*> mLivesIcons;
    int mMaxLives;

    const int TEXT_HEIGHT = 26;
    const int POINT_SIZE = 22;
};
