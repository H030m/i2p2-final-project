#include "BossEnemy.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Player/Player.hpp"
#include "Engine/LOG.hpp"
#include <iostream>
BossEnemy::BossEnemy(int id, float x, float y)
    : Enemy(id, "play/enemy-3.png", x, y, 64, 0, 500, 0) {
    type = 3;
    Tint = al_map_rgba(255, 100, 100, 255);  // Red tint for boss
    timeSinceLastDamage = 0.0f;
    Size.x = 128;
    Size.y = 128;
    
}

void BossEnemy::Update(float deltaTime) {
    Enemy::Update(deltaTime);
    
    if (!alive) return;

    // Update damage timer
    timeSinceLastDamage += deltaTime;
    std::cerr<<"UPdate Boss "<<timeSinceLastDamage<<"\n";
    // Deal damage to player when in range
    if (timeSinceLastDamage >= damageInterval) {
        timeSinceLastDamage = 0.0f;
        // std::cerr<<"UPdate Boss\n";
        PlayScene* scene = getPlayScene();
        if (!scene || !scene->player_dict.count(scene->my_id)) return;
        
        Player* player = scene->player_dict[scene->my_id];
        float distance = (player->Position - Position).Magnitude();
        
        if (distance <= 1024 + player->CollisionRadius) {
            player->TakeDamage(damageAmount);
            Engine::LOG(Engine::INFO) << "Boss damaged player!";
        }
    }
}

void BossEnemy::UpdateFromServer(float x, float y, float rotation, float hp, bool alive, float armor, bool stealth) {
    PlayScene* scene = getPlayScene();

    
    Enemy::UpdateFromServer(x, y, rotation, hp, alive, armor, stealth);
}

void BossEnemy::Draw() const {
    if (alive) {
        Enemy::Draw();
        
        if (PlayScene::DebugMode) {
            // al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(255, 0, 255), 3);
        }
    }
}