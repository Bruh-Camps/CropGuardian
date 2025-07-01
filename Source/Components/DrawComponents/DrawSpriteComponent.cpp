//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "DrawSpriteComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

DrawSpriteComponent::DrawSpriteComponent(class Actor* owner, const std::string &texturePath, const int width, const int height, const int drawOrder)
        :DrawComponent(owner, drawOrder)
        ,mWidth(width)
        ,mHeight(height)
        ,mScale(1.0f)
{
    mSpriteSheetSurface = mOwner->GetGame()->LoadTexture(texturePath);
}

DrawSpriteComponent::~DrawSpriteComponent()
{
    DrawComponent::~DrawComponent();

    if (mSpriteSheetSurface) {
        SDL_DestroyTexture(mSpriteSheetSurface);
        mSpriteSheetSurface = nullptr;
    }
}

void DrawSpriteComponent::Draw(SDL_Renderer *renderer, const Vector3 &modColor)
{
    const int scaledWidth = static_cast<int>(mWidth * mScale);
    const int scaledHeight = static_cast<int>(mHeight * mScale);

    const float ownerX = mOwner->GetPosition().x;
    const float ownerY = mOwner->GetPosition().y;
    const float cameraX = mOwner->GetGame()->GetCameraPos().x;
    const float cameraY = mOwner->GetGame()->GetCameraPos().y;

    SDL_Rect dstRect = {
        static_cast<int>(ownerX - (scaledWidth - mWidth) / 2.0f - cameraX),
        static_cast<int>(ownerY - (scaledHeight - mHeight) / 2.0f - cameraY),
        scaledWidth,
        scaledHeight
    };

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (mOwner->GetRotation() == Math::Pi) {
        flip = SDL_FLIP_HORIZONTAL;
    }

    SDL_SetTextureBlendMode(mSpriteSheetSurface, SDL_BLENDMODE_BLEND);

    SDL_SetTextureColorMod(mSpriteSheetSurface,
                           static_cast<Uint8>(modColor.x),
                           static_cast<Uint8>(modColor.y),
                           static_cast<Uint8>(modColor.z));

    SDL_RenderCopyEx(renderer, mSpriteSheetSurface, nullptr, &dstRect, mOwner->GetRotation(), nullptr, flip);
}
