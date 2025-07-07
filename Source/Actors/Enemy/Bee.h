//
// Created by fredb on 7/4/2025.
//

#pragma once

#include "Enemy.h"

class Bee : public Enemy
{
public:
    explicit Bee(class Game* game, float forwardSpeed = 100.0f, float deathTime = 0.4f, float life = 50.0f);

    void Kill() override;

private:
    void UpdateMovement(float deltaTime) override;

    float mOscillationTimer;
    float mOscillationFrequency;
    float mOscillationAmplitude;
};