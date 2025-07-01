//
// Created by Lucas N. Ferreira on 28/05/25.
//

#include "UIImage.h"

UIImage::UIImage(SDL_Renderer *renderer, const std::string &imagePath, const Vector2 &pos, const Vector2 &size, const Vector3 &color)
    : UIElement(pos, size, color),
    mTexture(nullptr),
    mRenderer(renderer)
{
    SDL_Surface* surface = IMG_Load(imagePath.c_str());

    if (!surface) {
        SDL_Log("Failed to load image: %s", IMG_GetError());
        return;
    }

    mTexture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface);

    if (!mTexture) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
    }
}

UIImage::~UIImage()
{
    if (mTexture) {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
    }
}

void UIImage::Draw(SDL_Renderer* renderer, const Vector2 &screenPos)
{
    if (!mTexture) return;

    SDL_Rect dest;
    dest.x = static_cast<int>(screenPos.x + mPosition.x);
    dest.y = static_cast<int>(screenPos.y + mPosition.y);
    dest.w = static_cast<int>(mSize.x);
    dest.h = static_cast<int>(mSize.y);

    SDL_RenderCopy(renderer, mTexture, nullptr, &dest);
}

void UIImage::SetTexture(const std::string& imagePath)
{
    if (mTexture) {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
    }

    SDL_Surface* surface = IMG_Load(imagePath.c_str());
    if (!surface) {
        SDL_Log("Failed to load image for SetTexture: %s", IMG_GetError());
        return;
    }

    mTexture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface);

    if (!mTexture) {
        SDL_Log("Failed to create texture for SetTexture: %s", SDL_GetError());
    }
}