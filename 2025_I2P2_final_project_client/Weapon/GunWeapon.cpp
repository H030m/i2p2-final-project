#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Bullet/FireBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "GunWeapon.hpp"
#include "Weapon.hpp"
#include "Scene/PlayScene.hpp"

GunWeapon::GunWeapon(float x, float y, int _owner_id)
    : Weapon("play/AK47_Sheet.png", x, y, 200, 0.1) {
    // Move center downward, since we the turret head is slightly biased upward.
    // Anchor.y += 8.0f / GetBitmapHeight();
    Anchor.x += 15.0f / GetBitmapHeight();
    owner_id = _owner_id;
    type = 1;
    SourceW = 135, SourceH = 51, SourceY = 0, SourceX = 0;
    Size.x = 135, Size.y = 51, Flip = 2;
}
void GunWeapon::CreateBullet() {
    Engine::Point diff = Engine::Point(cos(angle - ALLEGRO_PI / 2), sin(angle - ALLEGRO_PI / 2));
    float angle = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    // Change bullet position to the front of the gun barrel.
    getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 70, diff, angle, this));
    AudioHelper::PlayAudio("gun.wav");
}

void GunWeapon::Update(float deltaTime) {
    // gun animation 
    animation_tick += deltaTime;
    if (cos(Rotation) < 0)
        Flip = 2;
    else
        Flip = 0;
    ALLEGRO_MOUSE_STATE mouseState;
    al_get_mouse_state(&mouseState);
    if ((mouseState.buttons & 1)) {
        if (animation_tick > 0.01f) {
            animation_tick = 0;
            SourceX += SourceW;
        }
        if (SourceX > 6*SourceW) {
            SourceX = 0;
        }
    }
    else {
        SourceX = 0;
    }

    Weapon::Update(deltaTime);
}