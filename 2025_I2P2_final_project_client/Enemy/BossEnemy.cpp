#include "BossEnemy.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Player/Player.hpp"

BossEnemy::BossEnemy(int id, float x, float y)
    : Enemy(id, "play/enemy-3.png", x, y, 70, 0, 500, 0) {
    type = 3;
    Tint = al_map_rgba(255, 100, 100, 255);  // Red tint for boss
}

void BossEnemy::UpdateFromServer(float x, float y, float rotation, float hp, bool alive, float armor, bool stealth, int heal) {
    // TODO Heal player
    PlayScene * scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());

    scene->player_dict[scene->my_id]->Heal(heal);
    Enemy::UpdateFromServer(x, y, rotation, hp, alive, armor, stealth, heal);
}

void BossEnemy::Draw() const {
    if (alive) {
        Enemy::Draw();
        
        if (PlayScene::DebugMode) {
            // al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(255, 0, 255), 3);
        }
    }
}