#ifndef BOSSENEMY_HPP
#define BOSSENEMY_HPP

#include "Enemy.hpp"
#include "Engine/AudioHelper.hpp"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>

class BossEnemy : public Enemy {
private:
    struct AttackAnimation {
        float radius;
        float maxRadius;
        float progress; // 0-1
        bool active;
    };

    std::vector<AttackAnimation> attackAnimations;
    float attackRadius = 512.0f; // Max attack range
    float animationDuration = 0.5f; // Time for animation to complete
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> attackSound = nullptr;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> hitSound = nullptr;

public:
    float damageInterval = 5.0f;
    float timeSinceLastDamage;
    float damageAmount = 30.0f;
    
    BossEnemy(int id, float x, float y);
    void UpdateFromServer(float x, float y, float rotation, float hp, bool alive, float armor, bool stealth) override;
    void Draw() const override;
    void Update(float deltaTime) override;
};

#endif // BOSSENEMY_HPP
