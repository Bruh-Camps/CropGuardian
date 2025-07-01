#pragma once

#include "Actor.h"

class Base : public Actor {
public:
    explicit Base(Game* game, int lives, int coins);
    void DecreaseCoinsBy(int coins);
    void IncreaseCoinsBy(int coins);
    int GetCoins() {return mCoins;};

    void OnUpdate(float deltaTime) override;
private:
    class DrawAnimatedComponent* mDrawComponent;
    int mLives;
    int mCoins;
};
