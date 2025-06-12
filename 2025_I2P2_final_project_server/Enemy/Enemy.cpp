#include "Enemy.hpp"
#include <cmath>
#include <nlohmann/json.hpp>
#include "Engine/Point.hpp"
Enemy::Enemy(int type, int id, Engine::Point position, Engine::Point spawn, float radius, float speed, float hp, float damage, int money) 
    : type(type), id(id), position(position), spawn(spawn), collisionRadius(radius), speed(speed), hp(hp), damage(damage), money(money) {
}

void Enemy::Update(float deltaTime) {
    if (!alive) return;
    
    // Basic movement
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;
}

void Enemy::Hit(float damage) {
    hp -= damage;
    if (hp <= 0) {
        alive = false;
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
        {"type", "-1"} 
    };
}