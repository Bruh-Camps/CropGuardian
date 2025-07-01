#include "EnemyPortal.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "Slime.h"
#include "../MainHUD.h"

EnemyPortal::EnemyPortal(Game* game, float spawnInterval, int numWaves, float timeBetweenWaves, float initialDelay)
        :Actor(game)
        ,mSpawnInterval(spawnInterval)
        ,mNumWaves(numWaves)
        ,mTimeBetweenWaves(timeBetweenWaves)
        ,mInitialDelay(initialDelay)
{
    /*mSprite = new DrawSpriteComponent(
        this,
        "../Assets/Sprites/EnemyPortal/EnemyPortal.png",
        64,
        64,
        20);*/

    mDrawComponent = new DrawAnimatedComponent(this,
                                          "../Assets/Sprites/EnemyPortal/Portal.png",
                                          "../Assets/Sprites/EnemyPortal/Portal.json",
                                          5);

    mDrawComponent->AddAnimation("Active", {0,1,2,3,4,5,6,7,8});

    //mRotation = Math::Pi;
    mDrawComponent->SetAnimation("Active");
    mDrawComponent->SetAnimFPS(10.0f);

    mGame->GetMainHUD()->SetWaveCount(0, mNumWaves);
}

void EnemyPortal::OnUpdate(float deltaTime) {
    // Espera um tempo inicial antes de começar a gerar os inimigos
    if (!mStarted)
    {

        if (mTimeSinceLastSpawn > 0) mGame->GetMainHUD()->SetTime(mInitialDelay - mTimeSinceLastSpawn);

        mTimeSinceLastSpawn += deltaTime;
        if (mTimeSinceLastSpawn >= mInitialDelay)
        {
            mStarted = true;
            mTimeSinceLastSpawn = 0.0f;
            mInWave = true;
        }
        return;
    }

    if (mCurrentWave > mNumWaves)
    {
        //SDL_Log("Todas as ondas de inimigos foram criadas!");
        //this->SetState(ActorState::Paused); // Faz com que o portal deixe de ser atualizado
        //mState = ActorState::Destroy;
        return;
    }

    mTimeSinceLastSpawn += deltaTime;

    if (mInWave)
    {
        // Mostra no HUD qual é a onda atual de inimigos
        mGame->GetMainHUD()->SetWaveCount(mCurrentWave, mNumWaves);

        // Gerar inimigo individualmente
        if (mTimeSinceLastSpawn >= mSpawnInterval && mEnemiesSpawnedThisWave < mEnemiesPerWave)
        {
            auto slime = new Slime(mGame, 50); //30
            Vector2 pos = Vector2(GetPosition().x + (Game::TILE_SIZE/2), GetPosition().y + (Game::TILE_SIZE/2));
            slime->SetPosition(pos);
            slime->GetComponent<RigidBodyComponent>()->SetApplyGravity(false);


            mEnemiesSpawnedThisWave++;
            mTimeSinceLastSpawn = 0.0f;
        }

        // Se já gerou todos os inimigos da wave atual
        if (mEnemiesSpawnedThisWave >= mEnemiesPerWave)
        {
            mInWave = false;
            mTimeSinceLastSpawn = 0.0f; // Zera para contar o intervalo entre waves
        }
    }
    else
    {
        // Esperar tempo entre waves
        if (mTimeSinceLastSpawn >= mTimeBetweenWaves)
        {
            mCurrentWave++;
            mEnemiesSpawnedThisWave = 0;
            mInWave = true;
            mTimeSinceLastSpawn = 0.0f;
        }

        if (mTimeSinceLastSpawn > 0 && mCurrentWave < mNumWaves) mGame->GetMainHUD()->SetTime(mTimeBetweenWaves - mTimeSinceLastSpawn);
    }
}

