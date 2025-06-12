#include "Move.hpp"
#include "Enemy.hpp"
#include "Connect/RenderSender.hpp"

#include <algorithm>
#include <cmath>
#include "Engine/Point.hpp"
void UpdateEnemyInstance(Enemy& enemy, float deltaTime, RenderSender& sender) {
    if (!enemy.alive) return;
    
    // Find the closest player
    float minDistance = std::numeric_limits<float>::max();
    Engine::Point targetPosition(0,0);
    bool foundPlayer = false;
    
    {
        std::lock_guard<std::mutex> lock(sender.clientMutex);
        for (const auto& ctx : sender.clients) {
            if (ctx->active && sender.frame.contains(std::to_string(ctx->id))) {
                auto playerData = sender.frame[std::to_string(ctx->id)]["player"];
                if (playerData.is_array() && playerData.size() >= 2) {
                    Engine::Point playerPos((float)playerData[0], (float)playerData[1]);
                    float distance = std::hypot(enemy.position.x - playerPos.x, enemy.position.y - playerPos.y);
                    if (distance < minDistance) {
                        minDistance = distance;
                        targetPosition = playerPos;
                        foundPlayer = true;
                    }
                }
            }
        }
    }
    
    if (!foundPlayer) {
        // No players found, idle
        enemy.velocity = Engine::Point(0, 0);
        return;
    }
    
    // Calculate direction to player
    Engine::Point direction = targetPosition - enemy.position;
    if (direction.Magnitude() > 0) {
        direction = direction.Normalize();
    }
    
    // Update velocity and position
    enemy.velocity = direction * enemy.speed;
    enemy.position.x += enemy.velocity.x * deltaTime;
    enemy.position.y += enemy.velocity.y * deltaTime;
    
    // Update rotation for facing direction
    enemy.rotation = std::atan2(direction.y, direction.x) * 180.0f / M_PI;
    
    // Boundary check
    enemy.position.x = std::max(0.0f, std::min(enemy.position.x, static_cast<float>(sender.storedMapState.value()["map"]["MapWidth"]) * blockSize));
    enemy.position.y = std::max(0.0f, std::min(enemy.position.y, static_cast<float>(sender.storedMapState.value()["map"]["MapHeight"]) * blockSize));
}