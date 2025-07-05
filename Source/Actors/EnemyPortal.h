#pragma once

#include "Actor.h"

class EnemyPortal : public Actor
{
public:
    explicit EnemyPortal(Game* game, int numWaves, float timeBetweenWaves, float initialDelay);

    void OnUpdate(float deltaTime) override;

    bool AreAllWavesFinished() const { return mAllWavesFinished; }
private:
    class DrawSpriteComponent* mSprite;
    class DrawAnimatedComponent* mDrawComponent;

    int mNumWaves;              // número total de waves
    float mTimeBetweenWaves;    // tempo de espera entre waves
    float mInitialDelay;

    float mSpawnInterval;       // tempo entre inimigos
    int mEnemiesPerWave;        // quantos inimigos por wave
    float mBeeSpawnChance;      // chance de spawnar uma abelha

    float mTimeSinceLastSpawn = 0;      // contador de tempo
    int mCurrentWave = 1;               // wave atual
    int mEnemiesSpawnedThisWave = 0;
    bool mInWave = true;                // indica se está dentro de uma wave ou esperando
    bool mStarted = false;

    bool mAllWavesFinished = false;

    void SetupWaveParameters();
};

