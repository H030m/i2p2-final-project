#ifndef BOSSENEMY_HPP
#define BOSSENEMY_HPP

#include "Enemy.hpp"

class BossEnemy : public Enemy {
public:
    float damageInterval = 10.0f;
    float timeSinceLastDamage;
    float damageAmount = 50.0f;
    
    BossEnemy(int id, float x, float y);
    void UpdateFromServer(float x, float y, float rotation, float hp, bool alive, float armor, bool stealth) override;
    void Draw() const override;
    void Update(float deltaTime) override;
};

#endif // BOSSENEMY_HPP