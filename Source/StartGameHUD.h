#pragma once

#include "UIElements/UIScreen.h"

class StartGameHUD : public UIScreen
{
public:
    StartGameHUD(class Game* game, const std::string& fontName, SDL_Renderer* renderer);

    void Show();
    void Hide();
    void Draw(SDL_Renderer* renderer) override;
    //void ProcessInput(const uint8_t* state) override;

    bool IsVisible() const { return mIsVisible; }

private:
    bool mIsVisible;
    class UIText* mStartMessage;
    class UIImage* mBackgroundPanel;
};
