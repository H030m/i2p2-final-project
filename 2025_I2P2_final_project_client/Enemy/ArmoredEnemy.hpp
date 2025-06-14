#ifndef ARMOREDENEMY_HPP
#define ARMOREDENEMY_HPP
#include "Enemy.hpp"

class ArmoredEnemy : public Enemy {
protected:
    float armor;
public:
    ArmoredEnemy(float x, float y);
    void Hit(float damage) override;
    ArmoredEnemy(int id, float x, float y);
    void Draw() const override;
    void UpdateFromServer(float x, float y, float rotation, float hp, bool alive, float armor, bool stealth, int heal) override;
};
#endif