#ifndef ARMOREDENEMY_HPP
#define ARMOREDENEMY_HPP
#include "Enemy.hpp"

class ArmoredEnemy : public Enemy {
protected:
    float armor;
public:
    ArmoredEnemy(std::string img, float x, float y, float radius, float speed, float hp, int money, float armor);
    void Hit(float damage) override;
};
#endif