#ifndef ARMOREDENEMY_HPP
#define ARMOREDENEMY_HPP
#include "Enemy.hpp"

class ArmoredEnemy : public Enemy {
protected:
    float armor;
public:
    ArmoredEnemy(float x, float y);
    void Hit(float damage) override;
};
#endif