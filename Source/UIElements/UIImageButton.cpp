//
// Created by fredb on 6/11/2025.
//

#include "UIImageButton.h"
#include <SDL_image.h>

UIImageButton::UIImageButton(SDL_Renderer* renderer, const std::string& imagePath, std::function<void()> onClick,
                             const Vector2& pos, const Vector2& size, const Vector3& color)
        : UIElement(pos, size, color)
        , mTexture(nullptr)
        , mRenderer(renderer)
        , mOnClick(onClick)
{
    SetTexture(imagePath);
}

UIImageButton::~UIImageButton() {
    if (mTexture) {
        SDL_DestroyTexture(mTexture);
    }
}

void UIImageButton::SetTexture(const std::string& imagePath) {
    if (mTexture) {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
    }

    SDL_Surface* surface = IMG_Load(imagePath.c_str());
    if (!surface) {
        SDL_Log("Failed to load image button: %s", IMG_GetError());
        return;
    }

    mTexture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface);

    if (!mTexture) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
    }
}

void UIImageButton::Draw(SDL_Renderer* renderer, const Vector2& screenPos) {
    if (!mTexture) return;

    SDL_Rect dst;
    dst.x = static_cast<int>(screenPos.x + mPosition.x);
    dst.y = static_cast<int>(screenPos.y + mPosition.y);
    dst.w = static_cast<int>(mSize.x);
    dst.h = static_cast<int>(mSize.y);

    SDL_RenderCopy(renderer, mTexture, nullptr, &dst);
}

void UIImageButton::OnClick() {
    if (mOnClick) {
        mOnClick();
    }
}
