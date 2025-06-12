#ifndef STEALTHENEMY_HPP
#define STEALTHENEMY_HPP
#include "Enemy.hpp"

class StealthEnemy : public Enemy {
protected:
    bool stealth;
public:
    StealthEnemy(float x, float y);
    void Hit(float damage) override;
    void Update(float deltaTime) override;
    void Draw() const override;
};
#endif