#include <allegro5/base.h>
#include <cmath>
#include <string>
//TODO Change bullet
#include "Bullet/LaserBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "AbsorptionTurret.hpp"
#include "Scene/PlayScene.hpp"

const int AbsorptionTurret::Price = 10;
AbsorptionTurret::AbsorptionTurret(float x, float y) : Turret("play/tower-base.png", "play/absorption-turret.png", x, y, 300, Price, 0.1) {
    Anchor.y += 0.0f / GetBitmapHeight();
}
void AbsorptionTurret::CreateBullet() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    Engine::Point normal = Engine::Point(-normalized.y, normalized.x);
    // Change bullet position to the front of the gun barrel.
    getPlayScene()->BulletGroup->AddNewObject(new LaserBullet(Position + normalized * 36 - normal * 6, diff, rotation, this));
    getPlayScene()->BulletGroup->AddNewObject(new LaserBullet(Position + normalized * 36 + normal * 6, diff, rotation, this));
    AudioHelper::PlayAudio("laser.wav");
}
