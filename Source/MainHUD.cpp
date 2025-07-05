#include "MainHUD.h"
#include "Game.h"
#include "UIElements/UIText.h"
#include "UIElements/UIImage.h"
#include "UIElements/UIImageButton.h"

MainHUD::MainHUD(class Game *game, const std::string &fontName, SDL_Renderer *renderer)
    : UIScreen(game, fontName, renderer)
      ,mTimeText(nullptr)
      ,mTimeIcon(nullptr)
      ,mMaxLives(0)
{
    float windowWidth = mGame->GetWindowWidth();
    float windowHeight = mGame->GetWindowHeight();

     // === CONSTS LOCAIS ===
    const float bannerWidth = 320.0f;
    const float bannerHeight = 60.0f;
    const float iconSize = 48.0f;
    const float pointSize = POINT_SIZE;
    const float textHeight = TEXT_HEIGHT;
    const float margin = 20.0f;
    const float spacing = 20.0f;
    const float textIconSpacing = 8.0f;
    const float leftSideStartX = margin;

    // === POSICIONAMENTO DO BANNER E TEXTO DO NOME DA FASE ===
    Vector2 bannerSize(bannerWidth, bannerHeight);
    Vector2 bannerPos((windowWidth - bannerWidth) / 2.0f, margin);
    Vector2 mapTextSize(250.0f, textHeight);
    Vector2 mapTextPos(bannerPos.x + 20.0f, bannerPos.y + (bannerHeight - textHeight) / 2.0f);

    mMapNameText = AddText("  Corn fields", mapTextPos, mapTextSize, pointSize);
    mMapNameText->SetColor(Vector3(0, 0, 0));
    mMapNameText->SetText("  Corn fields");
    mBannerImage = AddImage("../Assets/Sprites/HUD/banner.png", bannerPos, bannerSize);

    float alignedY = bannerPos.y + (bannerHeight - iconSize) / 2.0f;
    float currentRightX = windowWidth - margin;

    // ONDA
    Vector2 waveIconSize(iconSize, iconSize);
    std::string waveStr = "1 / 10";
    float waveTextWidth = waveStr.length() * (pointSize * 0.5f);

    Vector2 waveTextSize(waveTextWidth, textHeight);
    Vector2 waveTextPos(currentRightX - waveTextWidth, alignedY + (iconSize - textHeight) / 2.0f);
    mWaveCountText = AddText(waveStr, waveTextPos, waveTextSize, pointSize);

    Vector2 waveIconPos(waveTextPos.x - iconSize - textIconSpacing, alignedY);
    mWaveIcon = AddImage("../Assets/Sprites/HUD/swords.png", waveIconPos, waveIconSize);

    currentRightX = waveIconPos.x - spacing;

    // MOEDA
    Vector2 coinIconSize(iconSize, iconSize);
    std::string coinStr = "100";
    float coinTextWidth = coinStr.length() * (pointSize * 0.6f);

    Vector2 coinTextSize(coinTextWidth, textHeight);
    Vector2 coinTextPos(currentRightX - coinTextWidth, alignedY + (iconSize - textHeight) / 2.0f);
    mCoinCountText = AddText(coinStr, coinTextPos, coinTextSize, pointSize);

    Vector2 coinIconPos(coinTextPos.x - iconSize - textIconSpacing, alignedY);
    mCoinIcon = AddImage("../Assets/Sprites/HUD/coin.png", coinIconPos, coinIconSize);

    currentRightX = coinIconPos.x - spacing;

    // TEMPO
    float timeIconSize = iconSize;
    std::string initialTime = "000";
    float timeTextWidth = initialTime.length() * (pointSize * 0.6f);

    Vector2 timeTextSize(timeTextWidth, textHeight);
    Vector2 timeTextPos(currentRightX - timeTextWidth, alignedY + (iconSize - textHeight) / 2.0f);
    mTimeText = AddText(initialTime, timeTextPos, timeTextSize, pointSize);

    Vector2 timeIconPos(timeTextPos.x - timeIconSize - textIconSpacing, alignedY);
    mTimeIcon = AddImage("../Assets/Sprites/HUD/time.png", timeIconPos, Vector2(timeIconSize, timeIconSize));

    // PAUSE
    mPauseButton = AddImageButton("../Assets/Sprites/HUD/pause.png",
    Vector2(10.0f, 10.0f), Vector2(64.0f, 64.0f),
    [this]() {
        mGame->TogglePause();
    });

    // VIDAS (Mesma linha do Pause, à direita dele)
    float livesIconSize = 32.0f;
    float livesSpacing = 5.0f;
    float livesAlignedY = mPauseButton->GetPosition().y + (mPauseButton->GetSize().y - livesIconSize) / 2.0f;
    float currentLivesX = mPauseButton->GetPosition().x + mPauseButton->GetSize().x + spacing;

    // LEVEL
    std::string levelStr = "Level " + std::to_string(mLevel);
    float levelPointSize = POINT_SIZE;
    float levelTextHeight = TEXT_HEIGHT;
    float levelTextWidth = levelStr.length() * (levelPointSize * 0.6f);

    Vector2 levelTextSize(levelTextWidth, levelTextHeight);
    Vector2 levelTextPos((windowWidth - levelTextWidth) / 2.0f, windowHeight - levelTextHeight - 10.0f);

    mLevelText = AddText(levelStr, levelTextPos, levelTextSize, levelPointSize);
    mLevelText->SetColor(Vector3(255, 255, 255));
}

MainHUD::~MainHUD() {}

void MainHUD::SetMapName(const std::string& name) {
    mMapNameText->SetText(name);
}

void MainHUD::SetLevel(int level) {
    mLevel = level;
    mLevelText->SetText("Level " + std::to_string(mLevel));
}

void MainHUD::SetCoinCount(int count) {
    const float pointSize = POINT_SIZE;
    const float textHeight = TEXT_HEIGHT;
    const float textIconSpacing = 8.0f;

    std::string countStr = std::to_string(count);
    mCoinCountText->SetText(countStr);

    float textWidth = countStr.length() * (pointSize * 0.6f);
    Vector2 iconPos = mCoinIcon->GetPosition();
    Vector2 newTextPos(iconPos.x + mCoinIcon->GetSize().x + textIconSpacing, iconPos.y + (mCoinIcon->GetSize().y - textHeight) / 2.0f);

    mCoinCountText->SetPosition(newTextPos);
    mCoinCountText->SetSize(Vector2(textWidth, textHeight));
}


void MainHUD::SetWaveCount(int currentWave, int totalWaves) {
    const float pointSize = POINT_SIZE;
    const float textHeight = TEXT_HEIGHT;
    const float textIconSpacing = 8.0f;

    std::string waveStr = std::to_string(currentWave) + " / " + std::to_string(totalWaves);
    mWaveCountText->SetText(waveStr);

    float textWidth = waveStr.length() * (pointSize * 0.5f);
    Vector2 iconPos = mWaveIcon->GetPosition();
    Vector2 newTextPos(iconPos.x + mWaveIcon->GetSize().x + textIconSpacing, iconPos.y + (mWaveIcon->GetSize().y - textHeight) / 2.0f);

    mWaveCountText->SetPosition(newTextPos);
    mWaveCountText->SetSize(Vector2(textWidth, textHeight));
}

void MainHUD::UpdatePauseButtonIcon(bool isPaused) {
    std::string newIcon = isPaused ? "../Assets/Sprites/HUD/play.png"
                                   : "../Assets/Sprites/HUD/pause.png";
    mPauseButton->SetTexture(newIcon);
}

void MainHUD::SetTime(float floatTime) {

    int time = static_cast<int>(floatTime);

    const float textIconSpacing = 8.0f;

    if (mTimeText) {
        const float pointSize = POINT_SIZE;
        const float textHeight = TEXT_HEIGHT;

        std::string timeStr = std::to_string(time);

        float timeTextWidth = timeStr.length() * (pointSize * 0.6f);
        Vector2 iconPos = mTimeIcon->GetPosition();
        Vector2 newTextPos(iconPos.x + mTimeIcon->GetSize().x + textIconSpacing, iconPos.y + (mTimeIcon->GetSize().y - textHeight) / 2.0f);

        mTimeText->SetText(timeStr);
        mTimeText->SetPosition(newTextPos);
        mTimeText->SetSize(Vector2(timeTextWidth, textHeight));
    }
}

void MainHUD::SetLives(int currentLives, int maxLives) {
    const float spacing = 20.0f;;
    if (maxLives != -1 && maxLives != mMaxLives) {
        mLivesIcons.clear();

        mMaxLives = maxLives;
        float livesIconSize = 32.0f;
        float livesSpacing = 5.0f;
        float livesAlignedY = mPauseButton->GetPosition().y + (mPauseButton->GetSize().y - livesIconSize) / 2.0f;
        float currentLivesX = mPauseButton->GetPosition().x + mPauseButton->GetSize().x + spacing;

        for (int i = 0; i < mMaxLives; ++i) {
            UIImage* heart = AddImage("../Assets/Sprites/HUD/heart_full.png",
                                     Vector2(currentLivesX, livesAlignedY),
                                     Vector2(livesIconSize, livesIconSize));
            mLivesIcons.push_back(heart);
            currentLivesX += livesIconSize + livesSpacing;
        }
    }

    for (int i = 0; i < mMaxLives; ++i) {
        if (mLivesIcons[i]) {
            if (i < currentLives) {
                mLivesIcons[i]->SetTexture("../Assets/Sprites/HUD/heart_full.png");
            } else {
                mLivesIcons[i]->SetTexture("../Assets/Sprites/HUD/heart_empty.png");
            }
        }
    }
}