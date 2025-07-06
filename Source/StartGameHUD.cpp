#include "StartGameHUD.h"
#include "Game.h"
#include "UIElements/UIText.h"
#include "UIElements/UIImage.h"

StartGameHUD::StartGameHUD(Game* game, const std::string& fontName, SDL_Renderer* renderer)
    : UIScreen(game, fontName, renderer)
    , mIsVisible(false)
{
    float windowWidth = mGame->GetWindowWidth();
    float windowHeight = mGame->GetWindowHeight();

    const Vector2 panelSize = Vector2(800, 500);
    Vector2 panelPos((windowWidth - panelSize.x) / 2.0f, (windowHeight - panelSize.y) / 2.0f);

    mBackgroundPanel = AddImage("../Assets/Sprites/HUD/panel.png", panelPos, panelSize);

    std::string message = "Defend the crops!\n\nBuild towers to stop the enemies.\n\nPress ENTER to start or continue.";
    Vector2 textPos = panelPos + Vector2(190.0f, 170.0f); // ajuste conforme fonte
    Vector2 mapTextSize(panelSize.x - 400, panelSize.y - 350);
    mStartMessage = AddText(message, textPos, mapTextSize); // texto branco

}

void StartGameHUD::Show()
{
    mIsVisible = true;

}

void StartGameHUD::Hide()
{
    mIsVisible = false;
}

void StartGameHUD::Draw(SDL_Renderer* renderer)
{
    if (!mIsVisible)
        return;

    UIScreen::Draw(renderer);
}

