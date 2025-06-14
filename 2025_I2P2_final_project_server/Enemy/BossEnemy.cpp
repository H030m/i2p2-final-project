#include "BossEnemy.hpp"
#include "Connect/RenderSender.hpp"

BossEnemy::BossEnemy(int id, Engine::Point position, Engine::Point spawn)
    : Enemy(0, id, position, spawn, 70, 0, 1500, 0, 0) {  // Type 3 for boss, radius and hp is bigger, speed and damage = 0 so it doesn't hurt players, money is irrelevent
    damageInterval = 10.0f;
    timeSinceLastDamage = 0.0f;
}

// TODO I passed RenderSender& sender to Update and Hit, so Enemy and all other enemies' both methods need to have this parameter as well
void BossEnemy::Update(float deltaTime, RenderSender& sender) {
    if (!alive) {
        // Not sure if we have to switch scene here too, it should switch when hp <= 0
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
        // TODO Go to submit scene, ask name/highschool
    }
}

nlohmann::json BossEnemy::Serialize() const {
    auto json = Enemy::Serialize();
    json["type"] = "-1";
    json["enemyType"] = 0;
    json["max_hp"] = 1500;
    // json["heal"]; // TODO client Enemy::UpdateFromServer has to recieve heal too (add a parameter?) and actually heal the player
    return json;
}