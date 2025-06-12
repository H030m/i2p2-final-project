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

ShotgunWeapon::ShotgunWeapon(float x, float y, int _owner_id)
    : Weapon("play/Shotgun1-sheet.png", x, y, 200, 0.7) {
    // Move center downward, since we the turret head is slightly biased upward.
    // Anchor.y += 8.0f / GetBitmapHeight();
    owner_id =  _owner_id;
    Anchor.x -= 25.0f / GetBitmapHeight();
    Size.x = 128, Size.y = 56;
    type = 2;
    SourceW = 62, SourceH = 28, SourceY = 0, SourceX = 0;
    delta = ALLEGRO_PI;
}
void ShotgunWeapon::CreateBullet() {
    const float spread = ALLEGRO_PI / 18; 
    const float speed = 10;
    float baseAngle = angle - ALLEGRO_PI / 2;
    for (int i = -3; i <= 3; ++i) {
        float angle = baseAngle + i * spread;
        Engine::Point diff = Engine::Point(cos(angle), sin(angle)) * speed;
        angle = atan2(diff.y, diff.x);
        Engine::Point normalized = diff.Normalize();
        Engine::Point spawnPos = Position + normalized*50;
        getPlayScene()->BulletGroup->AddNewObject(new FireBullet(spawnPos, diff, angle, this));
    }

    AudioHelper::PlayAudio("gun.wav");
}
void ShotgunWeapon::Update(float deltaTime) {
    // shotgun animation 
    animation_tick += deltaTime;
    if (cos(Rotation) < 0)
        Flip = 2;
    else
        Flip = 0;

    ALLEGRO_MOUSE_STATE mouseState;
    al_get_mouse_state(&mouseState);
    if ((mouseState.buttons & 1)) {
        SourceY = SourceH * 1;
        if (animation_tick > 0.14f) {
            animation_tick = 0;
            SourceX += SourceW;
        }
        if (SourceX > 4*SourceW) {
            SourceX = 0;
            SourceY += SourceH;
        }
        if (SourceY > SourceH*3) {
            SourceY = SourceH*1;
            SourceX = 0;
        }
    }
    else {
        SourceX = 0;
        SourceY = 0;
    }

    Weapon::Update(deltaTime);
}