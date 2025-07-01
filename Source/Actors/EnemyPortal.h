#pragma once

#include "Actor.h"

class EnemyPortal : public Actor
{
public:
    explicit EnemyPortal(Game* game, float spawnInterval, int numWaves, float timeBetweenWaves, float initialDelay);

    void OnUpdate(float deltaTime) override;
private:
    class DrawSpriteComponent* mSprite;
    class DrawAnimatedComponent* mDrawComponent;

    float mSpawnInterval;          // tempo entre inimigos
    float mTimeBetweenWaves;  // tempo de espera entre waves
    float mTimeSinceLastSpawn = 0; // contador de tempo
    int mNumWaves;                 // número total de waves
    int mCurrentWave = 1;          // wave atual
    int mEnemiesPerWave = 3;       // quantos inimigos por wave
    int mEnemiesSpawnedThisWave = 0;
    bool mInWave = true;           // indica se está dentro de uma wave ou esperando
    float mInitialDelay;
    bool mStarted = false;

};

