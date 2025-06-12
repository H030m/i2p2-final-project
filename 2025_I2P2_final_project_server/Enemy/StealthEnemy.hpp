#ifndef STEALTHENEMY_HPP
#define STEALTHENEMY_HPP
#include "Enemy.hpp"
#include <nlohmann/json.hpp>

class StealthEnemy : public Enemy {
private:
    bool stealth;
    float stealthCooldown = 0;
    
public:
    StealthEnemy(int id, Engine::Point position, Engine::Point spawn);
    void Update(float deltaTime) override;
    void Hit(float damage) override;
    nlohmann::json Serialize() const override;
};
#endif