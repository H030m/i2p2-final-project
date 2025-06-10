// #include <allegro5/base.h>
// #include <cmath>
// #include <string>

// #include "Bullet/LaserBullet.hpp"
// #include "Engine/AudioHelper.hpp"
// #include "Engine/Group.hpp"
// #include "Engine/Point.hpp"
// #include "Shovel.hpp"
// #include "Scene/PlayScene.hpp"

// const int Shovel::Price = 0;
// Shovel::Shovel(float x, float y) : Turret("play/shovel.png", "play/shovel.png", x, y, 300, Price, 0.5) {
//     // Move center downward, since we the turret head is slightly biased upward.
//     Anchor.y += 0;
// }
// void Shovel::CreateBullet() {
//     Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
//     float rotation = atan2(diff.y, diff.x);
//     Engine::Point normalized = diff.Normalize();
//     Engine::Point normal = Engine::Point(-normalized.y, normalized.x);
//     // Change bullet position to the front of the gun barrel.
//     getPlayScene()->BulletGroup->AddNewObject(new LaserBullet(Position + normalized * 36 - normal * 6, diff, rotation, this));
//     getPlayScene()->BulletGroup->AddNewObject(new LaserBullet(Position + normalized * 36 + normal * 6, diff, rotation, this));
//     AudioHelper::PlayAudio("laser.wav");
// }
