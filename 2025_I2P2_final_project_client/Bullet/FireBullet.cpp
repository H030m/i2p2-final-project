#include <allegro5/base.h>
#include <random>
#include <string>

#include "Enemy/Enemy.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "FireBullet.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/DirtyEffect.hpp"

class Weapon;

FireBullet::FireBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Weapon *parent) : Bullet("play/Bullet_1.png", 800 + parent->level*75, 20 + parent->level*10, position, forwardDirection, rotation + ALLEGRO_PI / 2, parent) {
    Size.x = 32 * (1+parent->level*0.5); Size.y = 32 * (1+parent->level*0.5);
    CollisionRadius = 16 * (1+parent->level*0.5);
}
void FireBullet::OnExplode(Enemy *enemy) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
    getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-1.png", dist(rng), enemy->Position.x, enemy->Position.y));
}
