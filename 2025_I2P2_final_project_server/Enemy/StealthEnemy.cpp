#include "StealthEnemy.hpp"
#include <cmath>
#include <nlohmann/json.hpp>
#include <iostream>

StealthEnemy::StealthEnemy(int id, Engine::Point position, Engine::Point spawn)
    : Enemy(2, id, position, spawn, 10, 10, 10, 10, 10) {
    stealth = true;
}

void StealthEnemy::Revive() {
    stealth = initStealth;
    hp = initHP;
    Enemy::Revive();
}

void StealthEnemy::Update(float deltaTime) {
    if (!alive && cooldown <= 0) {
        StealthEnemy::Revive();
    }
    
    Enemy::Update(deltaTime);
    
    if (!stealth) {
        stealthCooldown -= deltaTime;
        if (stealthCooldown <= 0) {
            stealth = true;
            stealthCooldown = 5.0f; // Re-stealth after 5 seconds
        }
    }
}

void StealthEnemy::Hit(float damage) {
    if (stealth) {
        std::cerr << "StealthEnemy hit when stealth\n";
        stealth = false;
        stealthCooldown = 5.0f;
    }
    Enemy::Hit(damage);
}

nlohmann::json StealthEnemy::Serialize() const {
    auto json = Enemy::Serialize();
    json["type"] = "-1"; // Armored enemy type
    json["enemyType"] = 2;
    json["stealth"] = stealth;
    return json;
}