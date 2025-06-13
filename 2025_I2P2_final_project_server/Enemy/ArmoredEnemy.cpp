#include "ArmoredEnemy.hpp"
#include <nlohmann/json.hpp>

ArmoredEnemy::ArmoredEnemy(int id, Engine::Point position, Engine::Point spawn)
    : Enemy(1, id, position, spawn, 10, 10, 10, 10, 10) {
}

void ArmoredEnemy::Hit(float damage) {
    // Apply damage reduction from armor
    float effectiveDamage = damage * (1.0f - armor / 100.0f);
    Enemy::Hit(effectiveDamage);
}

nlohmann::json ArmoredEnemy::Serialize() const {
    auto json = Enemy::Serialize();
    json["type"] = "-1";
    json["enemyType"] = 1;
    json["armor"] = armor;
    return json;
}