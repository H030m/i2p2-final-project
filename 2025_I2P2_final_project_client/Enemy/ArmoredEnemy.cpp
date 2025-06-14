#include <string>

#include "ArmoredEnemy.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Connect/Client.hpp"
ArmoredEnemy::ArmoredEnemy(int id, float x, float y)
    : Enemy(id, "play/enemy-1.png", x, y, 10, 10, 10, 10) {
    armor = 50;
    type = 1;
    Tint = al_map_rgba(150, 150, 255, 255);
}
void ArmoredEnemy::Draw() const {
    if (alive) {
        Enemy::Draw();
    }
}
void ArmoredEnemy::Hit(float damage) {
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    GameClient &sender = game.sender;
    sender.output_json["Hit"].push_back({
        {"id",id},
        {"Damage",damage},
        {"HitVx",HitV.x},
        {"HitVy",HitV.y}
    });
}
void ArmoredEnemy::UpdateFromServer(float x, float y, float rotation, float hp, bool alive, float armor, bool stealth, int heal) {
    this->armor = armor;
    Enemy::UpdateFromServer(x, y, rotation, hp, alive, armor, stealth, heal);
}