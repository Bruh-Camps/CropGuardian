//
// Created by fredb on 6/11/2025.
//

#include "BuildTowerHUD.h"
#include "Game.h"
#include "Actors/BuildSpot.h"
#include "Actors/NormalTower.h"
#include "UIElements/UIImage.h"
#include "UIElements/UIImageButton.h"


BuildTowerHUD::BuildTowerHUD(class Game* game, const std::string& fontName, SDL_Renderer* renderer)
    : UIScreen(game, fontName, renderer)
    , mActiveBuildSpot(nullptr)
    , mIsVisible(false)
{
    float windowWidth = mGame->GetWindowWidth();
    float windowHeight = mGame->GetWindowHeight();

    const float panelWidth = 580.0f;
    const float panelHeight = 200.0f;
    const float buttonSize = 150.0f;
    const float margin = 20.0f;
    const float spacing = 20.0f;

    // === PAINEL DE FUNDO ===
    Vector2 panelPos(windowWidth - panelWidth - margin, windowHeight - panelHeight - margin);
    // Podemos adicionar esse painel de fundo depois, mas acho que está bom sem
    // mBackgroundPanel = AddImage("../Assets/Sprites/HUD/panel_build.png", panelPos, Vector2(panelWidth, panelHeight));

    // === BOTÕES ===
    float startX = panelPos.x + spacing;
    float buttonY = panelPos.y + (panelHeight - buttonSize) / 2.0f;

    mTower1 = AddImageButton(
        "../Assets/Sprites/HUD/sample_tower_banner.png",
        Vector2(startX, buttonY),
        Vector2(buttonSize, buttonSize),
        [this]() {
            if (mActiveBuildSpot)
            {
                if (mGame->GetCurrentBase()->GetCoins()>=75) {
                    mGame->GetCurrentBase()->DecreaseCoinsBy(75);
                    mActiveBuildSpot->BuildTower(BuildSpot::TowerType::Normal);
                    Hide();
                }
            }
        });

    startX += buttonSize + spacing;
    mTower2 = AddImageButton(
        "../Assets/Sprites/HUD/empty_tower_banner.png",
        Vector2(startX, buttonY),
        Vector2(buttonSize, buttonSize),
        [this]() {
            if (mActiveBuildSpot)
            {
                //mActiveBuildSpot->BuildTower(/* TowerType::Magic*/);
                Hide();
            }
        });

    startX += buttonSize + spacing;
    mTower3 = AddImageButton(
        "../Assets/Sprites/HUD/empty_tower_banner.png",
        Vector2(startX, buttonY),
        Vector2(buttonSize, buttonSize),
        [this]() {
            if (mActiveBuildSpot)
            {
                //mActiveBuildSpot->BuildTower(/* TowerType::Artillery */);
                Hide();
            }
        });

    Hide();
}

void BuildTowerHUD::Show(BuildSpot* activeSpot)
{
    if (!mIsVisible || mActiveBuildSpot != activeSpot) {
        mActiveBuildSpot = activeSpot;
        mIsVisible = true;
    }
    else {
        Hide();
    }

}

void BuildTowerHUD::Hide()
{
    mActiveBuildSpot = nullptr;
    mIsVisible = false;
}

void BuildTowerHUD::Draw(SDL_Renderer* renderer)
{
    if (!mIsVisible)
    {
        return;
    }
    UIScreen::Draw(renderer);
}