#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>
#include <random>
#include <string>
#include <vector>

#include "Bullet/Bullet.hpp"
#include "Enemy.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IScene.hpp"
#include "Engine/LOG.hpp"
#include "Scene/PlayScene.hpp"
#include "Turret/Turret.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/ExplosionEffect.hpp"

PlayScene *Enemy::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}
void Enemy::OnExplode() {
    getPlayScene()->EffectGroup->AddNewObject(new ExplosionEffect(Position.x, Position.y));
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> distId(1, 3);
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 20);
    for (int i = 0; i < 10; i++) {
        // Random add 10 dirty effects.
        getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-" + std::to_string(distId(rng)) + ".png", dist(rng), Position.x, Position.y));
    }
}
Enemy::Enemy(int id, std::string img, float x, float y, float radius, float speed, float hp, int money) 
    : Engine::Sprite(img, x, y), id(id), speed(speed), hp(hp), money(money) {
    CollisionRadius = radius;
    reachEndTime = 0;
}
void Enemy::UpdateFromServer(float x, float y, float rotation, float hp, bool alive) {
    Position.x = x;
    Position.y = y;
    Rotation = rotation;
    this->hp = hp;
    this->alive = alive;
    
    if (!alive && this->alive) { // Just died
        OnExplode();
        getPlayScene()->EarnMoney(money);
        getPlayScene()->EnemyGroup->RemoveObject(objectIterator);
    }
}

void Enemy::Update(float deltaTime) {
    // Client-side enemies no movement logic
    Sprite::Update(deltaTime);
}
void Enemy::Hit(float damage) {
    /*
    hp -= damage;
    if (hp <= 0) {
        getPlayScene()->EarnMoney(money);
        getPlayScene()->EnemyGroup->RemoveObject(objectIterator);
        AudioHelper::PlayAudio("explosion.wav");
    }
    */
}



void Enemy::Draw() const {
    Sprite::Draw();
    if (PlayScene::DebugMode) {
        // Draw collision radius.
        al_draw_circle(Position.x, Position.y, 150, al_map_rgb(255, 0, 0), 2);
    }
}