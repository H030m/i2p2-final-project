#include "BossEnemy.hpp"
#include "Connect/RenderSender.hpp"

BossEnemy::BossEnemy(int id, Engine::Point position, Engine::Point spawn)
    : Enemy(0, id, position, spawn, 70, 0, 3000, 0, 0) {  // Type 0 for boss, radius and hp is bigger, speed and damage = 0 so it doesn't hurt players, money is irrelevent
    damageInterval = 10.0f;
    timeSinceLastDamage = 0.0f;
}

void BossEnemy::Update(float deltaTime, RenderSender& sender) {
    if (!alive) {
        return;
    }

    // Update damage timer
    timeSinceLastDamage += deltaTime;
    
    // Deal damage to all nearby players every 10 seconds
    if (timeSinceLastDamage >= damageInterval) {
        timeSinceLastDamage = 0.0f;
        
        // Get all players from RenderSender
        std::lock_guard<std::mutex> lock(sender.clientMutex);
        for (auto& ctx : sender.clients) {
            if (ctx->active && ctx->lastInput.contains("player")) {
                auto playerData = ctx->lastInput["player"];
                if (playerData.is_array() && playerData.size() == 6) {
                    Engine::Point playerPos((float)playerData[0], (float)playerData[1]);
                    float distance = (playerPos - position).Magnitude();
                    
                    if (distance <= collisionRadius + 20) { // TODO Player's collisionRadius should be passed here, I forgot whether playerData has collisionRadius
                        ctx->lastInput["damageTaken"] = damageAmount;
                    }
                }
            }
        }
    }
}

void BossEnemy::Hit(float damage, RenderSender& sender) {
    // Don't call Enemy::Hit() for boss, switch scene after death instead
    hp -= damage;
    if (hp <= 0) {
        alive = false;
    }
}

nlohmann::json BossEnemy::Serialize() const {
    auto json = Enemy::Serialize();
    json["type"] = "-1";
    json["enemyType"] = 0;
    json["max_hp"] = 1500;
    return json;
}