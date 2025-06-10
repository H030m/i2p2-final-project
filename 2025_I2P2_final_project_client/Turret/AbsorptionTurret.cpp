// #include <allegro5/base.h>
// #include <cmath>
// #include <string>
// //TODO Change bullet
// #include "Bullet/FireNetBullet.hpp"
// #include "Engine/AudioHelper.hpp"
// #include "Engine/Group.hpp"
// #include "Engine/Point.hpp"
// #include "AbsorptionTurret.hpp"
// #include "Scene/PlayScene.hpp"

// const int AbsorptionTurret::Price = 100;

// AbsorptionTurret::AbsorptionTurret(float x, float y) : Turret("play/tower-base.png", "play/absorption-turret.png", x, y, 200, Price, 1) {
//     Anchor.y += 0.0f / GetBitmapHeight();
//     aim_front = false;
// }
// void AbsorptionTurret::CreateBullet() {
//     Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
//     float rotation = atan2(diff.y, diff.x);
//     Engine::Point normalized = diff.Normalize();
//     Engine::Point normal = Engine::Point(-normalized.y, normalized.x);
//     // Change bullet position to the front of the gun barrel.
//     getPlayScene()->BulletGroup->AddNewObject(new FireNetBullet(Position + normalized * 36 - normal * 6, diff, rotation, this));
//     getPlayScene()->BulletGroup->AddNewObject(new FireNetBullet(Position + normalized * 36 + normal * 6, diff, rotation, this));
//     AudioHelper::PlayAudio("laser.wav");
// }
