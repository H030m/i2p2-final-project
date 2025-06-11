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
<<<<<<< HEAD
=======

>>>>>>> 99b35bcf769e3dedb1dc8ef8dc6a0985c2d7badc
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

<<<<<<< HEAD
    // 1. ���N�ù��y���Y�񬰹C���y�С]�B�z���ù�/��j�Y�p�^Add commentMore actions
    float screenX = mouseState.x / ((float)al_get_display_width(game.display) / game.screenW);
    float screenY = mouseState.y / ((float)al_get_display_height(game.display) / game.screenH);

    // 2. �� Camera ��ù��y���ର�@�ɮy��
    Engine::Point worldMousePos = getPlayScene()->GetCamera()->ScreenToWorld(Engine::Point(screenX, screenY));

    // 3. �p��P�Z�����Z����V�]�@�ɮy�ФU�^
=======
    // 1. 先將螢幕座標縮放為遊戲座標（處理全螢幕/放大縮小）
    float screenX = mouseState.x / ((float)al_get_display_width(game.display) / game.screenW);
    float screenY = mouseState.y / ((float)al_get_display_height(game.display) / game.screenH);

    // 2. 用 Camera 把螢幕座標轉為世界座標
    Engine::Point worldMousePos = getPlayScene()->GetCamera()->ScreenToWorld(Engine::Point(screenX, screenY));

    // 3. 計算與武器的距離方向（世界座標下）
>>>>>>> 99b35bcf769e3dedb1dc8ef8dc6a0985c2d7badc
    float dx = worldMousePos.x - Position.x;
    float dy = worldMousePos.y - Position.y;

    this->Rotation = atan2(dy, dx);
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