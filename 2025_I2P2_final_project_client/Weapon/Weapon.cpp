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
    return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}

Weapon::Weapon(std::string imgWeapon, float x, float y, float radius, float coolDown) : Sprite(imgWeapon, x, y) , coolDown(coolDown) {
    rotateRadian = 0.0f;
}

void Weapon::Update(float deltaTime) {
    Player* player = getPlayScene()->player_dict[getPlayScene()->my_id];
    Position.x  = player->Position.x;
    Position.y = player->Position.y + 20;
    Sprite::Update(deltaTime);

    ALLEGRO_MOUSE_STATE mouseState;
    al_get_mouse_state(&mouseState);

    float dx = mouseState.x - Position.x;
    float dy = mouseState.y - Position.y;

    this->Rotation = atan2(dy, dx) + ALLEGRO_PI / 2;

    if (!aim_front) {
        this->Rotation += ALLEGRO_PI;
    }

    //create bullet
    if ((mouseState.buttons & 1) && reload <= 0) {
        CreateBullet();
        reload = coolDown; 
    }

    if (reload > 0) {
        reload -= deltaTime;
    }
}


void Weapon::Draw() const {
    Sprite::Draw();
}