#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Bullet/OrbitBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "CircleWeapon.hpp"
#include "Weapon.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"
CircleWeapon::CircleWeapon(float x, float y, int _owner_id, int _level)
    : Weapon("play/turret-1.png", x, y, 200, 0.1) {
    // Move center downward, since we the turret head is slightly biased upward.
    // Anchor.y += 8.0f / GetBitmapHeight();
    created = false;
    owner_id =  _owner_id;
    type = 3;
    level = _level;
}
void CircleWeapon::CreateBullet() {
    // Change bullet position to the front of the gun barrel.
    Player* player = getPlayScene()->player_dict[getPlayScene()->my_id];
    int bulletCount = 4 + (level - 1); // Level 1 = 4, Level 2 = 5, ..., Level 5 = 8

    for (int i = 0; i < bulletCount; i++) {
        float angle = 2 * ALLEGRO_PI * i / bulletCount;
        getPlayScene()->BulletGroup->AddNewObject(new OrbitBullet(angle, 100 + 20*level, 4, player, this));
    }
    // AudioHelper::PlayAudio("gun.wav");
}

void CircleWeapon::Update(float Deltatime) {
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();

    if (!created && game.my_id == owner_id) {
        this->CreateBullet();
        created = true;
    } 
}

void CircleWeapon::Draw() const {
    //don't draw
}