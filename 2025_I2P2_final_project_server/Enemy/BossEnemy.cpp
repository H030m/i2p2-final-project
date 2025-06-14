#include "BossEnemy.hpp"
#include "Connect/RenderSender.hpp"

BossEnemy::BossEnemy(int id, Engine::Point position, Engine::Point spawn)
    : Enemy(3, id, position, spawn, 70, 0, 500, 0, 0) {  // Type 3 for boss, radius and hp is bigger, speed and damage = 0 so it doesn't hurt players, money is irrelevent
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
            if (ctx->active && sender.frame.contains(std::to_string(ctx->id))) {
                auto playerData = sender.frame[std::to_string(ctx->id)]["player"];
                if (playerData.is_array() && playerData.size() >= 2) {
                    Engine::Point playerPos((float)playerData[0], (float)playerData[1]);
                    float distance = (playerPos - position).Magnitude();
                    
                    if (distance <= collisionRadius + 20) {
                        sender.frame[std::to_string(ctx->id)]["damageTaken"] = damageAmount;
                    }
                }
            }
        }
    }
}

void BossEnemy::Hit(float damage, RenderSender& sender) {
    auto it = sender.Hitenemy.find(id);
    // Find the attacking player from hit information
    if (it != sender.Hitenemy.end()) {
        for (auto& hitInfo : it->second) {
            if (sender.frame.contains(std::to_string(hitInfo.player_id))) {
                sender.frame[std::to_string(hitInfo.player_id)]["healReceived"] = healAmount;
            }
        }
    }
    
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
    json["enemyType"] = 3;
    json["heal"] = healAmount; // TODO client Enemy::UpdateFromServer has to recieve heal too (add a parameter?) and actually heal the player
    return json;
}