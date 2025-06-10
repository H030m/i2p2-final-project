#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>
#include <utility>
#include <algorithm>

#include "Enemy/Enemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IObject.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Scene/PlayScene.hpp"
#include "Weapon.hpp"

PlayScene* Weapon::getPlayScene() {
    return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetCurrentScene());
}

Weapon::Weapon(std::string imgWeapon, float x, float y, float radius, float coolDown) : Sprite(imgWeapon, x, y) , coolDown(coolDown) {
    rotateRadian = 0.0f;
}

void Weapon::Update(float deltaTime) {
    Sprite::Update(deltaTime);

    ALLEGRO_MOUSE_STATE mouseState;
    al_get_mouse_state(&mouseState);

    float dx = mouseState.x - this->x;
    float dy = mouseState.y - this->y;

    this->Rotation = atan2(dy, dx);

    if (!aim_front) {
        this->Rotation += ALLEGRO_PI;
    }
}


void Weapon::Draw() {
    Sprite::Draw();
}