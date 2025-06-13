#ifndef ARMOREDENEMY_HPP
#define ARMOREDENEMY_HPP
#include "Enemy.hpp"

class ArmoredEnemy : public Enemy {
private:

    const float initArmor = 50;
    const float initRadius = 10;
    const float initSpeed = 10;
    const float initHP = 10;
    const float initDamage = 10;
    const float initMoney = 10;
public:
    float armor = 50;
    ArmoredEnemy(int id, Engine::Point position, Engine::Point spawn);
    void Hit(float damage) override;
    nlohmann::json Serialize() const override;
    void Revive() override;
    void Update(float deltaTime) override;
};
#endif