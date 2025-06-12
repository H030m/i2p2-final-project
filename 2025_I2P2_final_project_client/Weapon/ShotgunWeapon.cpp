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

ShotgunWeapon::ShotgunWeapon(float x, float y, int _owner_id, int _level)
    : Weapon("play/Shotgun1-sheet.png", x, y, 200, 0.7) {
    // Move center downward, since we the turret head is slightly biased upward.
    // Anchor.y += 8.0f / GetBitmapHeight();
    Anchor.x -= 25.0f / GetBitmapHeight();
    Size.x = 128, Size.y = 56;
    SourceW = 62, SourceH = 28, SourceY = 0, SourceX = 0;
    delta = ALLEGRO_PI;
    owner_id =  _owner_id;
    type = 2;
    level = _level;
}
void ShotgunWeapon::CreateBullet() {
    const float spread = ALLEGRO_PI / 18; 
    const float speed = 10;
    float baseAngle = angle - ALLEGRO_PI / 2;
    int bulletcount = 2 + level;
    for (int i = -1*bulletcount; i <= bulletcount; ++i) {
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
    if (owner_id == getPlayScene()->my_id) {
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
    }
    

    Weapon::Update(deltaTime);
}