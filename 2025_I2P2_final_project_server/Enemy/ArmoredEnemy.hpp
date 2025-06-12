#ifndef ARMOREDENEMY_HPP
#define ARMOREDENEMY_HPP
#include "Enemy.hpp"

class ArmoredEnemy : public Enemy {
private:
    float armor = 50;
    
public:
    ArmoredEnemy(int id, Engine::Point position, Engine::Point spawn);
    void Hit(float damage) override;
    nlohmann::json Serialize() const override;
};
#endif