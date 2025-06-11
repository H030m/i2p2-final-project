#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Bullet/BounceBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "BounceWeapon.hpp"
#include "Weapon.hpp"
#include "Scene/PlayScene.hpp"

BounceWeapon::BounceWeapon(float x, float y)
    : Weapon("play/rpg.png", x, y, 200, 0.5) {
    // Move center downward, since we the turret head is slightly biased upward.
    // Anchor.y += 8.0f / GetBitmapHeight();
    Size.x = 78, Size.y = 32;
}
void BounceWeapon::CreateBullet() {
    Engine::Point diff = Engine::Point(cos(angle - ALLEGRO_PI / 2), sin(angle - ALLEGRO_PI / 2));
    float angle = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    // Change bullet position to the front of the gun barrel.
    getPlayScene()->BulletGroup->AddNewObject(new BounceBullet(Position + normalized * 36, diff, angle + ALLEGRO_PI / 2, this));
    AudioHelper::PlayAudio("gun.wav");
}

void BounceWeapon::Update(float deltaTime) {
    if (cos(Rotation) < 0)
        Flip = 2;
    else
        Flip = 0;

    Weapon::Update(deltaTime);
}