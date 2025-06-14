#ifndef ARMOREDENEMY_HPP
#define ARMOREDENEMY_HPP
#include "Enemy.hpp"

class ArmoredEnemy : public Enemy {
private:

    static const float initArmor;
    static const float initRadius;
    static const float initSpeed;
    static const float initHP;
    static const float initDamage;
    static const float initMoney;
public:
    float armor = 50;
    ArmoredEnemy(int id, Engine::Point position, Engine::Point spawn);
    void Hit(float damage) override;
    nlohmann::json Serialize() const override;
    void Revive() override;
    void Update(float deltaTime) override;
};
#endif