#include "ArmoredEnemy.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
ArmoredEnemy::ArmoredEnemy(int id, Engine::Point position, Engine::Point spawn)
    : Enemy(1, id, position, spawn, initRadius, initSpeed, initHP, initDamage, initMoney) {
    armor = initArmor;
}

void ArmoredEnemy::Revive() {
    armor = initArmor;
    hp = initHP;
    Enemy::Revive();
}

void ArmoredEnemy::Update(float deltaTime) {
    if (!alive && cooldown <= 0) {
        ArmoredEnemy::Revive();
    }
    Enemy::Update(deltaTime);
}

void ArmoredEnemy::Hit(float damage) {
    // Apply damage reduction from armor
    float remainingArmor = armor - damage;
    std::cerr<<"armor! "<<armor<<'\n';
    if (remainingArmor < 0) {
        Enemy::Hit(-remainingArmor);
        armor = 0;
    } else {
        armor = remainingArmor;
    }
}

nlohmann::json ArmoredEnemy::Serialize() const {
    auto json = Enemy::Serialize();
    json["type"] = "-1";
    json["enemyType"] = 1;
    json["armor"] = armor;
    return json;
}