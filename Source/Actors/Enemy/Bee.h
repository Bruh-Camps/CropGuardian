//
// Created by fredb on 7/4/2025.
//

#pragma once

#include "Enemy.h"

class Bee : public Enemy
{
public:
    explicit Bee(class Game* game, float forwardSpeed = 120.0f, float deathTime = 0.4f, float life = 50.0f);

private:
    void UpdateMovement(float deltaTime) override;

    float mOscillationTimer;
    float mOscillationFrequency;
    float mOscillationAmplitude;
};