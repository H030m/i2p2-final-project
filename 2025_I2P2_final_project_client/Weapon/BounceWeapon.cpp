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
    : Weapon("play/turret-1.png", x, y, 200, 0.5) {
    // Move center downward, since we the turret head is slightly biased upward.
    // Anchor.y += 8.0f / GetBitmapHeight();
}
void BounceWeapon::CreateBullet() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    // Change bullet position to the front of the gun barrel.
    getPlayScene()->BulletGroup->AddNewObject(new BounceBullet(Position + normalized * 36, diff, rotation + ALLEGRO_PI / 2, this));
    AudioHelper::PlayAudio("gun.wav");
}