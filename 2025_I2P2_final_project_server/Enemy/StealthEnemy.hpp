#ifndef STEALTHENEMY_HPP
#define STEALTHENEMY_HPP
#include "Enemy.hpp"
#include <nlohmann/json.hpp>

class StealthEnemy : public Enemy {
private:
    bool stealth;
    float stealthCooldown = 0;
    const float initStealth = true;
    const float initRadius = 10;
    const float initSpeed = 10;
    const float initHP = 10;
    const float initDamage = 10;
    const float initMoney = 10;
public:
    StealthEnemy(int id, Engine::Point position, Engine::Point spawn);
    void Update(float deltaTime) override;
    void Revive() override;
    void Hit(float damage) override;
    nlohmann::json Serialize() const override;
};
#endif