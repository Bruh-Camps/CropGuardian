//
// Created by fredb on 6/11/2025.
//

#pragma once
#include "UIElement.h"
#include <functional>
#include <string>
#include <SDL.h>

class UIImageButton : public UIElement {
public:
    UIImageButton(SDL_Renderer* renderer, const std::string& imagePath, std::function<void()> onClick,
                  const Vector2& pos, const Vector2& size, const Vector3& color = Vector3(1.f, 1.f, 1.f));
    ~UIImageButton();

    void Draw(SDL_Renderer* renderer, const Vector2& screenPos) override;
    void OnClick();

    void SetTexture(const std::string& imagePath);

    void SetHighlighted(bool highlighted) { mHighlighted = highlighted; }

private:
    SDL_Texture* mTexture;
    SDL_Renderer* mRenderer;
    std::function<void()> mOnClick;

    bool mHighlighted = false;
};
