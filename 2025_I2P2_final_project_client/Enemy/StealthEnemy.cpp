#include <allegro5/allegro_primitives.h>
#include "StealthEnemy.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"

StealthEnemy::StealthEnemy(float x, float y)
    : Enemy("play/enemy-2.png", x, y, 10, 10, 10, 10), stealth(true) {
}

void StealthEnemy::Hit(float damage) {
    if (stealth) {
        stealth = false; // Reveal when hit by a bullet
        Engine::LOG(Engine::INFO) << "StealthEnemy revealed by bullet hit";
    }
    hp -= damage;
    if (hp <= 0) {
        OnExplode();
        getPlayScene()->EarnMoney(money);
        getPlayScene()->EnemyGroup->RemoveObject(objectIterator);
    }
}

void StealthEnemy::Update(float deltaTime) {
    // Check for collision with any player in PlayerGroup
    PlayScene* scene = getPlayScene();
    if (stealth && scene->PlayerGroup) {
        for (auto& playerObj : scene->PlayerGroup->GetObjects()) {
            auto* player = dynamic_cast<Player*>(playerObj);
            if (player) {
                Engine::Point diff = player->Position - Position;
                float distance = diff.Magnitude();
                if (distance <= CollisionRadius + player->CollisionRadius) {
                    stealth = false; // Reveal when colliding with any player
                    Engine::LOG(Engine::INFO) << "StealthEnemy revealed by player collision at (" << Position.x << ", " << Position.y << ")";
                    break; // Stop checking after revealing
                }
            }
        }
    }
    Enemy::Update(deltaTime); // Call base class Update for movement and other logic
}

void StealthEnemy::Draw() const {
    if (!stealth) {
        Enemy::Draw(); // Only draw if not stealthed
    }
    if (PlayScene::DebugMode) {
        // Draw collision radius even if stealthed for debugging
        al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(255, 0, 0), 2);
    }
}