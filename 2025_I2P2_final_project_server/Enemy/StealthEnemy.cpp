#include "StealthEnemy.hpp"
#include <cmath>
#include <nlohmann/json.hpp>
#include <iostream>

const float StealthEnemy::initStealth = false;
const float StealthEnemy::initRadius = 32;
const float StealthEnemy::initSpeed = 40;
const float StealthEnemy::initHP = 700;
const float StealthEnemy::initDamage = 10;
const float StealthEnemy::initMoney = 10;
StealthEnemy::StealthEnemy(int id, Engine::Point position, Engine::Point spawn)
    : Enemy(2, id, position, spawn, initRadius, initSpeed, initHP, initDamage, initMoney) {
    stealth = true;
}

void StealthEnemy::Revive() {
    Enemy::Revive();
    stealth = initStealth;
    hp = initHP * (1 + (float)revive_num/3);
    speed = initSpeed * (1 + (float)revive_num/20.0);
    revive_cooldown *= (1 + 0.001);
}

void StealthEnemy::Update(float deltaTime, RenderSender& sender) {
    if (!alive && cooldown <= 0) {
        StealthEnemy::Revive();
    }
    
    Enemy::Update(deltaTime, sender);
    
    if (!stealth) {
        stealthCooldown -= deltaTime;
        if (stealthCooldown <= 0) {
            stealth = true;
            stealthCooldown = revive_cooldown; // Re-stealth after 5 seconds
        }
    }
}

void StealthEnemy::Hit(float damage, RenderSender& sender) {
    if (stealth) {
        // std::cerr << "StealthEnemy hit when stealth\n";
        stealth = false;
        stealthCooldown =  1.5f;
    }
    Enemy::Hit(damage, sender);
}

nlohmann::json StealthEnemy::Serialize() const {
    auto json = Enemy::Serialize();
    json["type"] = "-1"; // Armored enemy type
    json["enemyType"] = 2;
    json["stealth"] = stealth;
    json["max_hp"] = initHP * (1 + (float)revive_num/3);
    return json;
}