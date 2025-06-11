#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Bullet/FireBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "ShotgunWeapon.hpp"
#include "Weapon.hpp"
#include "Scene/PlayScene.hpp"

ShotgunWeapon::ShotgunWeapon(float x, float y)
    : Weapon("play/turret-3.png", x, y, 200, 0.7) {
    // Move center downward, since we the turret head is slightly biased upward.
    // Anchor.y += 8.0f / GetBitmapHeight();
}
void ShotgunWeapon::CreateBullet() {
    const float spread = ALLEGRO_PI / 18; // ¡Ó10 «× (radians)
    const float speed = 10;
    float baseAngle = Rotation - ALLEGRO_PI / 2;

    for (int i = -3; i <= 3; ++i) {
        float angle = baseAngle + i * spread;
        Engine::Point diff = Engine::Point(cos(angle), sin(angle)) * speed;
        float rotation = atan2(diff.y, diff.x);
        Engine::Point spawnPos = Position + diff.Normalize() * 36;

        getPlayScene()->BulletGroup->AddNewObject(new FireBullet(spawnPos, diff, rotation, this));
    }

    AudioHelper::PlayAudio("gun.wav");
}