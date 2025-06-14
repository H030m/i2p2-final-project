#ifndef BOSSENEMY_HPP
#define BOSSENEMY_HPP

#include <chrono>
#include <nlohmann/json.hpp>
#include "Engine/Point.hpp"
#include "Connect/RenderSender.hpp"
class BossEnemy : public Enemy {
private:
    float damageInterval;  // Time between damage ticks (10 seconds)
    float timeSinceLastDamage;  // Timer for damage ticks
    const float damageAmount = 40.0f;  // Damage dealt per tick
    const float healAmount = 2.0f;     // Healing when attacked
    
public:
    BossEnemy(int id, Engine::Point position, Engine::Point spawn);
    
    void Update(float deltaTime, RenderSender& sender) override;
    void Hit(float damage, RenderSender& sender) override;
    nlohmann::json Serialize() const override;
};

#endif // BOSSENEMY_HPP