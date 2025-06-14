#include "ArmoredEnemy.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
const float ArmoredEnemy::initArmor = 50;
const float ArmoredEnemy::initRadius = 32;
const float ArmoredEnemy::initSpeed = 50;
const float ArmoredEnemy::initHP = 100;
const float ArmoredEnemy::initDamage = 10;
const float ArmoredEnemy::initMoney = 10;
ArmoredEnemy::ArmoredEnemy(int id, Engine::Point position, Engine::Point spawn)
    : Enemy(1, id, position, spawn, initRadius, initSpeed, initHP, initDamage, initMoney) {
    armor = initArmor;
}


void ArmoredEnemy::Revive() {
    Enemy::Revive();
    armor = initArmor* (1 + (float)revive_num/3.0);
    hp = initHP * (1 + (float)revive_num/3.0);
    speed = initSpeed * (1 + (float)revive_num/100.0);
    revive_cooldown *= (1 + 0.001);
}

void ArmoredEnemy::Update(float deltaTime, RenderSender& sender) {
    if (!alive && cooldown <= 0) {
        ArmoredEnemy::Revive();
    }
    Enemy::Update(deltaTime, sender);
}

void ArmoredEnemy::Hit(float damage, RenderSender& sender) {
    // Apply damage reduction from armor
    float remainingArmor = armor - damage;
    std::cerr<<"armor! "<<armor<<'\n';
    if (remainingArmor < 0) {
        Enemy::Hit(-remainingArmor, sender);
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
    json["max_hp"] = initHP * (1 + (float)revive_num/3.0) + armor* (1 + (float)revive_num/3.0);
    return json;
}