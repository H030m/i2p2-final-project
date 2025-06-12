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

CircleWeapon::CircleWeapon(float x, float y, int _owner_id)
    : Weapon("play/turret-1.png", x, y, 200, 0.1) {
    // Move center downward, since we the turret head is slightly biased upward.
    // Anchor.y += 8.0f / GetBitmapHeight();
    created = false;
    owner_id =  _owner_id;
    type = 3;
}
void CircleWeapon::CreateBullet() {
    // Change bullet position to the front of the gun barrel.
    Player* player = getPlayScene()->player_dict[getPlayScene()->my_id];
    getPlayScene()->BulletGroup->AddNewObject(new OrbitBullet(0, 100, 4, player));
    getPlayScene()->BulletGroup->AddNewObject(new OrbitBullet(ALLEGRO_PI * 0.5, 100, 4, player));
    getPlayScene()->BulletGroup->AddNewObject(new OrbitBullet(ALLEGRO_PI, 100, 4, player));
    getPlayScene()->BulletGroup->AddNewObject(new OrbitBullet(ALLEGRO_PI * 1.5, 100, 4, player));
    // AudioHelper::PlayAudio("gun.wav");
}

void CircleWeapon::Update(float Deltatime) {
    if (!created) {
        this->CreateBullet();
        created = true;
    } 
}

void CircleWeapon::Draw() const {
    //don't draw
}