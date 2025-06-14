#include <allegro5/allegro_primitives.h>
#include "StealthEnemy.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"

StealthEnemy::StealthEnemy(int id, float x, float y)
    : Enemy(id, "play/enemy-2.png", x, y, 10, 10, 10, 10) {
    stealth = true;
    type = 2;
}

void StealthEnemy::Hit(float damage) {
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    GameClient &sender = game.sender;
    sender.output_json["Hit"].push_back({
        {"id",id},
        {"Damage",damage},
        {"HitVx",HitV.x},
        {"HitVy",HitV.y}
    });
}

void StealthEnemy::Update(float deltaTime) {
    /*
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
    }*/
    Enemy::Update(deltaTime); // Call base class Update for movement and other logic
}

void StealthEnemy::Draw() const {
    
    if (!stealth && alive) {
        Enemy::Draw(); // Only draw if not stealthed
        // std::cerr<<"hello "<<id<<'\n';
    }
    if (PlayScene::DebugMode) {
        // Draw collision radius even if stealthed for debugging
        al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(255, 0, 0), 2);
    }
}
void StealthEnemy::UpdateFromServer(float x, float y, float rotation, float hp, bool alive, float armor, bool stealth) {
    this->stealth = stealth;
    Enemy::UpdateFromServer(x, y, rotation, hp, alive, armor, stealth);
}