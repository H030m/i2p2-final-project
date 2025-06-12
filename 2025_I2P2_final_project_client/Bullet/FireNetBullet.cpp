#include <allegro5/base.h>
#include <random>
#include <string>

#include "Enemy/Enemy.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "FireNetBullet.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/DirtyEffect.hpp"

class Weapon;

FireNetBullet::FireNetBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Weapon *parent) : Bullet("play/fire-net.png", 800, 0, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent) {
    poison = true;
    CollisionRadius = 4;
}
void FireNetBullet::OnExplode(Enemy *enemy) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(2, 10);
    enemy->poison = 1;
    //DEBUGGER
    getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-2.png", dist(rng), enemy->Position.x, enemy->Position.y));
}
