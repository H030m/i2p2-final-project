#ifndef ENEMY_HPP
#define ENEMY_HPP
#include <list>
#include <string>
#include <vector>
#include "Engine/Point.hpp"
#include <nlohmann/json.hpp>
class Enemy {
public:

    int type;
    int id;
    float hp;
    float speed;
    float damage;
    int money;
    float collisionRadius;
    bool alive = true;
    float cooldown = 0;
    float hit_cooldown = 0;
    Engine::Point position;
    Engine::Point spawn;
    Engine::Point velocity;
    float rotation = 0;
    
    std::vector<Engine::Point> path;
    Engine::Point lastTarget;

    Enemy(int type, int id, Engine::Point position, Engine::Point spawn, float radius, float speed, float hp, float damage, int money);
    virtual ~Enemy() = default;
    
    // Core functionality
    virtual void Update(float deltaTime);
    virtual void Hit(float damage);
    virtual void Revive();
    // For network synchronization
    virtual nlohmann::json Serialize() const;
};
#endif