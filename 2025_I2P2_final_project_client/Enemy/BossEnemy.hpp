#ifndef BOSSENEMY_HPP
#define BOSSENEMY_HPP

#include "Enemy.hpp"

class BossEnemy : public Enemy {
private:
    struct AttackAnimation {
        float radius;
        float maxRadius;
        float progress; // 0-1
        bool active;
    };

    std::vector<AttackAnimation> attackAnimations;
    float attackRadius = 1024.0f; // Max attack range
    float animationDuration = 0.5f; // Time for animation to complete

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
