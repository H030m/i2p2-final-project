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
#include "Camera/Camera.hpp"
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
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();

    // 1. ���N�ù��y���Y�񬰹C���y�С]�B�z���ù�/��j�Y�p�^Add commentMore actions
    float screenX = mouseState.x / ((float)al_get_display_width(game.display) / game.screenW);
    float screenY = mouseState.y / ((float)al_get_display_height(game.display) / game.screenH);

    // 2. �� Camera ��ù��y���ର�@�ɮy��
    Engine::Point worldMousePos = getPlayScene()->GetCamera()->ScreenToWorld(Engine::Point(screenX, screenY));

    // 3. �p��P�Z�����Z����V�]�@�ɮy�ФU�^
    float dx = worldMousePos.x - Position.x;
    float dy = worldMousePos.y - Position.y;

    this->angle = atan2(dy, dx) + ALLEGRO_PI / 2;
    this->Rotation = atan2(dy, dx) + ALLEGRO_PI + delta;
    // while (Rotation < 2 * ALLEGRO_PI) Rotation -= 2*ALLEGRO_PI;

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