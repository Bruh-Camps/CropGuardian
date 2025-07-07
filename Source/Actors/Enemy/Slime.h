#pragma once

#include "SDL_render.h"
#include "../Actor.h"

#pragma once

#include "Enemy.h"

class Slime : public Enemy {
public:
    explicit Slime(class Game *game, float forwardSpeed = 50.0f, float deathTime = 0.5f, float life = 100.0f);

    void Kill() override;

private:
    void UpdateMovement(float deltaTime) override;
};
