#include "Base.h"

#include "Enemy/Slime.h"
#include "../Game.h"
#include "../MainHUD.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

Base::Base(Game *game, int lives, int coins)
    : Actor(game)
      , mLives(lives)
      , mCoins(coins){
    mDrawComponent = new DrawAnimatedComponent(this,
                                               "../Assets/Sprites/Base/Flag.png",
                                               "../Assets/Sprites/Base/Flag.json",
                                               20);

    mDrawComponent->AddAnimation("Active", {0, 1, 2, 3, 4, 5});
    mDrawComponent->SetAnimation("Active");
    mDrawComponent->SetAnimFPS(10.0f);

    // Atualiza o HUD inicial com o número de vidas e moedas
    mGame->GetMainHUD()->SetLives(lives);
    mGame->GetMainHUD()->SetCoinCount(mCoins);
}

void Base::DecreaseCoinsBy(int coins) {
    mCoins -= coins;
    mGame->GetMainHUD()->SetCoinCount(mCoins);
}

void Base::IncreaseCoinsBy(int coins) {
    mCoins += coins;
    mGame->GetMainHUD()->SetCoinCount(mCoins);
}

void Base::OnUpdate(float deltaTime) {

    if (mLives <= 0) {
        return;
    }

    // Verifica quais actors estão por perto
    Vector2 myPos = GetPosition();
    std::vector<Actor *> nearbyActors = mGame->GetNearbyActors(myPos, 1);

    for (Actor *actor: nearbyActors) {
        // Verifica se é um inimigo
        if (dynamic_cast<Slime *>(actor)) {
            Vector2 pos = actor->GetPosition();
            if (fabs(myPos.x - pos.x) < 10 && fabs(myPos.y - pos.y) < 50) {

                // Reduz vida
                mLives--;

                mGame->GetMainHUD()->SetLives(mLives);

                // Destroi o inimigo
                actor->SetState(ActorState::Destroy);

                // Verifica se as vidas acabaram
                if (mLives <= 0) {
                    SDL_Log("Game Over!");
                    mGame->SetGameScene(Game::GameScene::GameOver, 1.0f);
                    return;
                }
            }
        }
    }
}
