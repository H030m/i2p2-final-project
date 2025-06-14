#ifndef STEALTHENEMY_HPP
#define STEALTHENEMY_HPP
#include "Enemy.hpp"

class StealthEnemy : public Enemy {
protected:
    bool stealth;
public:
    StealthEnemy(int id, float x, float y);
    void UpdateFromServer(float x, float y, float rotation, float hp, bool alive, float armor, bool stealth, int heal) override;
    void Hit(float damage) override;
    void Update(float deltaTime) override;
    void Draw() const override;
};
#endif