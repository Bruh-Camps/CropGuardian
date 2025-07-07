// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "UIScreen.h"
#include "../Game.h"
#include "UIFont.h"

UIScreen::UIScreen(Game *game, const std::string &fontName, SDL_Renderer *renderer)
    : mGame(game)
      , mPos(0.f, 0.f)
      , mSize(0.f, 0.f)
      , mState(UIState::Active)
      , mSelectedButtonIndex(-1)
      , mRenderer(renderer) {
    mGame->PushUI(this);


    mFont = mGame->LoadFont(fontName);
}

UIScreen::~UIScreen() {
    for (auto text: mTexts) {
        delete text;
    }
    mTexts.clear();


    for (auto button: mButtons) {
        delete button;
    }
    mButtons.clear();


    for (auto image: mImages) {
        delete image;
    }
    mImages.clear();
}

void UIScreen::Update(float deltaTime) {
}

void UIScreen::Draw(SDL_Renderer *renderer) {
    for (auto image : mImages)
        image->Draw(renderer, mPos);

    for (auto button : mButtons)
        button->Draw(renderer, mPos);

    for (auto imgButton : mImageButtons)
        imgButton->Draw(renderer, mPos);

    for (auto text : mTexts)
        text->Draw(renderer, mPos);
}

void UIScreen::ProcessInput(const uint8_t *keys) {
}

void UIScreen::HandleKeyPress(int key) {
    switch (key) {
        case SDLK_w:
            if (!mButtons.empty()) {
                // Desmarca botão atual
                if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < static_cast<int>(mButtons.size())) {
                    mButtons[mSelectedButtonIndex]->SetHighlighted(false);
                }

                // Move para o botão anterior
                --mSelectedButtonIndex;
                if (mSelectedButtonIndex < 0) {
                    mSelectedButtonIndex = static_cast<int>(mButtons.size()) - 1;
                }

                // Destaca novo botão
                mButtons[mSelectedButtonIndex]->SetHighlighted(true);

                // Tocar som de navegação pra cima
                mGame->PlaySound("menu_move.wav", false);
            }
            break;

        case SDLK_s:
            if (!mButtons.empty()) {
                // Desmarca botão atual
                if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < static_cast<int>(mButtons.size())) {
                    mButtons[mSelectedButtonIndex]->SetHighlighted(false);
                }

                // Move para o próximo botão
                ++mSelectedButtonIndex;
                if (mSelectedButtonIndex >= static_cast<int>(mButtons.size())) {
                    mSelectedButtonIndex = 0;
                }

                // Destaca novo botão
                mButtons[mSelectedButtonIndex]->SetHighlighted(true);

                // Tocar som de navegação pra baixo
                mGame->PlaySound("menu_move.wav", false);
            }
            break;

        case SDLK_RETURN:
            if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < static_cast<int>(mButtons.size())) {
                mButtons[mSelectedButtonIndex]->OnClick();
            }
            break;

        default:
            break;
    }
}

void UIScreen::ProcessMouseClick(int mouseX, int mouseY)
{
    Vector2 mousePos(static_cast<float>(mouseX), static_cast<float>(mouseY));


    for (auto imgButton : mImageButtons)
    {
        Vector2 pos = imgButton->GetPosition() + mPos;
        Vector2 size = imgButton->GetSize();

        if (mousePos.x >= pos.x && mousePos.x <= pos.x + size.x &&
            mousePos.y >= pos.y && mousePos.y <= pos.y + size.y)
        {
            imgButton->OnClick();
            break;
        }
    }

    for (auto button : mButtons)
    {
        Vector2 pos = button->GetPosition() + mPos;
        Vector2 size = button->GetSize();

        if (mousePos.x >= pos.x && mousePos.x <= pos.x + size.x &&
            mousePos.y >= pos.y && mousePos.y <= pos.y + size.y)
        {
            button->OnClick();
            break;
        }
    }
}


void UIScreen::Close() {
    mState = UIState::Closing;
}

UIText *UIScreen::AddText(const std::string &name, const Vector2 &pos, const Vector2 &dims, const int pointSize,
                          const int unsigned wrapLength) {
    Vector3 white = Vector3(255, 255, 255);
    UIText *t = new UIText(name, mFont, pointSize, wrapLength, pos, dims, white);
    mTexts.push_back(t);
    return t;
}

UIButton *UIScreen::AddButton(const std::string &name, const Vector2 &pos, const Vector2 &dims,
                              std::function<void()> onClick) {
    auto *b = new UIButton(name, mFont, onClick, pos, dims, Vector3(255, 165, 0));
    mButtons.emplace_back(b);

    if (mButtons.size() == 1) {
        mSelectedButtonIndex = 0;
        b->SetHighlighted(true);
    }

    return b;
}

UIImage *UIScreen::AddImage(const std::string &imagePath, const Vector2 &pos, const Vector2 &dims,
                            const Vector3 &color) {
    auto *img = new UIImage(mRenderer, imagePath, pos, dims, color);
    mImages.emplace_back(img);
    return img;
}

UIImageButton* UIScreen::AddImageButton(const std::string& imagePath, const Vector2& pos, const Vector2& size, std::function<void()> onClick) {
    auto* button = new UIImageButton(mRenderer, imagePath, onClick, pos, size);
    mImageButtons.emplace_back(button);
    return button;
}

void UIScreen::ProcessMouseHover(int mouseX, int mouseY)
{
    Vector2 mousePos(static_cast<float>(mouseX), static_cast<float>(mouseY));

    UIImageButton* newHovered = nullptr;

    for (auto button : mImageButtons) {
        Vector2 pos = button->GetPosition() + mPos;
        Vector2 size = button->GetSize();
        if (mousePos.x >= pos.x && mousePos.x <= pos.x + size.x &&
            mousePos.y >= pos.y && mousePos.y <= pos.y + size.y)
        {
            newHovered = button;
            break;
        }
    }

    if (newHovered != mHoveredImageButton) {
        if (mHoveredImageButton) {
            mHoveredImageButton->SetHighlighted(false);
        }

        if (newHovered) {
            newHovered->SetHighlighted(true);
        }

        mHoveredImageButton = newHovered;
    }
}