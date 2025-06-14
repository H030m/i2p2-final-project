#include "Enemy.hpp"
#include <cmath>
#include <nlohmann/json.hpp>
#include "Engine/Point.hpp"
#include <iostream>
Enemy::Enemy(int type, int id, Engine::Point position, Engine::Point spawn, float radius, float speed, float hp, float damage, int money) 
    : type(type), id(id), position(position), spawn(spawn), collisionRadius(radius), speed(speed), hp(hp), damage(damage), money(money) {
}

void Enemy::Revive() {
    alive = true;
    position = spawn;
    revive_num++;
    path.clear();
}


void Enemy::Update(float deltaTime, RenderSender& sender) {
    // std::cerr<<"alive?" <<id<<' '<<alive<<'\n';
    
    if (!alive) {
        if (cooldown <= 0) Enemy::Revive();
        else cooldown -= deltaTime;
        return;
    }
    // Basic movement
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;
}

void Enemy::Hit(float damage, RenderSender& sender) {
    hp -= damage;
    // std::cerr<<"id hp "<<id<<' '<<hp<<'\n';
    if (hp <= 0) {
        alive = false;
        // std::cerr<<"die!"<< ' '<<alive<<'\n';
        cooldown = revive_cooldown;
    }
}

nlohmann::json Enemy::Serialize() const {
    return {
        {"id", id},
        {"x", position.x},
        {"y", position.y},
        {"rotation", rotation},
        {"hp", hp},
        {"alive", alive},
        {"enemyType", 0}, // Basic enemy type
        {"type", "-1"},
        {"armor", 0},
        {"stealth", false}
    };
}