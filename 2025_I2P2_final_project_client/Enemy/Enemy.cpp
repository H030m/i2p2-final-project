#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>
#include <random>
#include <string>
#include <vector>
#include <iostream>
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
    std::cerr<<"hello explode "<<'\n';
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
void Enemy::UpdateFromServer(float x, float y, float rotation, float hp, bool alive, float armor, bool stealth) {
    Position.x = x;
    Position.y = y;
    Rotation = rotation;
    this->hp = hp;
    if(!alive && expo_cooldown == 0){
        OnExplode();
        expo_cooldown = 100;
        std::cerr<<"shitt\n";
    }
    if(alive)expo_cooldown = 0;
    // std::cerr<<"hello alive? "<<this->alive<<' '<<alive<<'\n';
    if (!alive && this->alive) { // Just died
        
        // getPlayScene()->EarnMoney(money);
        // getPlayScene()->EnemyGroup->RemoveObject(objectIterator);
        this->Visible = false;
    }
    if (alive && !this->alive) { // Just revived
        // getPlayScene()->EnemyGroup->AddNewObject(this);
        this->Visible =true;
    }

    this->alive = alive;
    if (stealth == false && alive) {
        this->Visible = true;
    }
}

void Enemy::Update(float deltaTime) {
    // Client-side enemies no movement logic
    Sprite::Update(deltaTime);
}
void Enemy::Hit(float damage) {
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    GameClient &sender = game.sender;
    sender.output_json["Hit"].push_back({
        {"id",id},
        {"Damage",damage},
        {"HitVx",HitV.x},
        {"HitVy",HitV.y}
    });
}



void Enemy::Draw() const {
    Sprite::Draw();
    if (PlayScene::DebugMode) {
        // Draw collision radius.
        al_draw_circle(Position.x, Position.y, 150, al_map_rgb(255, 0, 0), 2);
    }
    if(type == 0)return;
    // health bar settings
    const float barWidth = 50.0f;
    const float barHeight = 6.0f;
    const float barOffsetY = -30.0f;  
    const int maxHealth = max_hp;
    
    // health bar position
    float barX = Position.x - barWidth / 2;
    float barY = Position.y + barOffsetY;
    
    // health bar percent
    float healthPercent = static_cast<float>(hp) / static_cast<float>(maxHealth);
    healthPercent = std::max(0.0f, std::min(1.0f, healthPercent));
    
    // health bar background (black)
    al_draw_filled_rectangle(
        barX - 1, barY - 1,
        barX + barWidth + 1, barY + barHeight + 1,
        al_map_rgb(40, 40, 40)
    );
    
    // change health bar color base on percent
    ALLEGRO_COLOR healthColor;
    if (healthPercent > 0.6f) {
        healthColor = al_map_rgb(0, 255, 0);      // green
    } else if (healthPercent > 0.3f) {
        healthColor = al_map_rgb(255, 255, 0);    // yellow
    } else {
        healthColor = al_map_rgb(255, 0, 0);      // red
    }
    
    // draw health bar
    if (healthPercent > 0) {
        al_draw_filled_rectangle(
            barX + 1, barY + 1,
            barX + (barWidth - 2) * healthPercent, barY + barHeight - 1,
            healthColor
        );
    }
}