#include <string>

#include "ArmoredEnemy.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Connect/Client.hpp"
ArmoredEnemy::ArmoredEnemy(float x, float y)
    : Enemy("play/enemy-1.png", x, y, 10, 10, 10, 10), armor(50) {
    Tint = al_map_rgba(150, 150, 255, 255);
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