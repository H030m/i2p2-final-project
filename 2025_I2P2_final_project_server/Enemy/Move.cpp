#include "Move.hpp"
#include "Enemy.hpp"
#include "Connect/RenderSender.hpp"

#include <algorithm>
#include <cmath>
#include <queue>
#include <vector>
#include <unordered_set>
#include <iostream>
#include "Engine/Point.hpp"

// Helper struct for A* pathfinding
struct Node {
    Engine::Point position;
    float gCost;  // Cost from start to current node
    float hCost;  // Heuristic cost to target
    float fCost() const { return gCost + hCost; }
    Node* parent = nullptr;
    
    bool operator<(const Node& other) const {
        return fCost() > other.fCost(); // For priority queue (min-heap)
    }
};

// Hash function for Point to use in unordered_set
namespace std {
    template<> struct hash<Engine::Point> {
        size_t operator()(const Engine::Point& p) const {
            return hash<float>()(p.x) ^ hash<float>()(p.y);
        }
    };
}

// Helper function to convert world position to grid coordinates
Engine::Point WorldToGrid(const Engine::Point& worldPos, int blockSize) {
    return Engine::Point(
        static_cast<int>(worldPos.x / blockSize),
        static_cast<int>(worldPos.y / blockSize)
    );
}

// Helper function to convert grid coordinates to world position
Engine::Point GridToWorld(const Engine::Point& gridPos, int blockSize) {
    return Engine::Point(
        gridPos.x * blockSize + blockSize / 2,
        gridPos.y * blockSize + blockSize / 2
    );
}

bool IsWalkable(const Engine::Point& gridPos, const nlohmann::json& map) {
    // Check bounds
    if (gridPos.x < 0 || gridPos.y < 0 || 
        gridPos.x >= (int)map[0].size() || gridPos.y >= (int)map.size()) {
        return false;
    }
    
    // Check if tile has an obstacle
    if (map[gridPos.y][gridPos.x].contains("Obstacle")) {
        // Only walkable if explicitly marked as penetrable
        return map[gridPos.y][gridPos.x]["Obstacle"].contains("Penetrable") && 
               map[gridPos.y][gridPos.x]["Obstacle"]["Penetrable"].get<bool>();
    }
    return true;
}

// A* pathfinding implementation
std::vector<Engine::Point> FindPath(const Engine::Point& start, const Engine::Point& target, 
                                   const nlohmann::json& mapState, int blockSize) {
    std::vector<Engine::Point> path;
    if (!mapState.contains("map") || !mapState["map"].contains("MapState")) return path;

    const auto& map = mapState["map"]["MapState"];


    Engine::Point startGrid = WorldToGrid(start, blockSize);
    Engine::Point targetGrid = WorldToGrid(target, blockSize);

    // Early exit if target is not walkable
    if (!IsWalkable(targetGrid, map)) {
        return path;
    }

    std::priority_queue<Node> openSet;
    std::unordered_set<Engine::Point> closedSet;
    std::unordered_map<Engine::Point, Node> allNodes;

    Node startNode;
    startNode.position = startGrid;
    startNode.gCost = 0;
    startNode.hCost = std::hypot(targetGrid.x - startGrid.x, targetGrid.y - startGrid.y);
    allNodes[startGrid] = startNode;
    openSet.push(startNode);

    const std::vector<Engine::Point> directions = {
        Engine::Point(-1, 0),  // Left
        Engine::Point(1, 0),   // Right
        Engine::Point(0, -1),  // Up
        Engine::Point(0, 1)    // Down
    };

    while (!openSet.empty()) {
        Node current = openSet.top();
        openSet.pop();

        if (current.position == targetGrid) {
            // Reconstruct path
            while (current.parent != nullptr) {
                path.push_back(GridToWorld(current.position, blockSize));
                current = *current.parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        closedSet.insert(current.position);

        for (const auto& dir : directions) {
            Engine::Point neighborPos = current.position + dir;
            
            // Skip if not walkable or already evaluated
            if (!IsWalkable(neighborPos, map) || closedSet.find(neighborPos) != closedSet.end()) {
                continue;
            }

            float moveCost = 1.0f; // Uniform cost for 4-direction movement
            float gCost = current.gCost + moveCost;
            
            // If new path to neighbor is better or neighbor isn't in open set

            if (allNodes.find(neighborPos) == allNodes.end() || gCost < allNodes[neighborPos].gCost) {
                Node neighbor;
                neighbor.position = neighborPos;
                neighbor.gCost = gCost;
                neighbor.hCost = std::hypot(targetGrid.x - neighborPos.x, targetGrid.y - neighborPos.y);
                neighbor.parent = &allNodes[current.position];
                
                allNodes[neighborPos] = neighbor;
                openSet.push(neighbor);
            }
        }
    }
    
    return path; // Empty if no path found
}


void UpdateEnemyInstance(Enemy& enemy, float deltaTime, RenderSender& sender) {
    // std::cerr<<enemy.id<<'\n';
    enemy.Update(deltaTime, sender);
    
    enemy.hit_cooldown -= deltaTime;
    if(enemy.hit_cooldown < 0) enemy.hit_cooldown = 0;
    // if (!enemy.alive) return;
    
    // Find the closest player
    float minDistance = std::numeric_limits<float>::max();
    Engine::Point targetPosition(0,0);
    bool foundPlayer = false;
    
    {
        std::lock_guard<std::mutex> lock(sender.clientMutex);
        for (const auto& ctx : sender.clients) {
            if (ctx->active) {
                // std::cerr<<"player! "<<ctx->id<<'\n';
                auto playerData = ctx->lastInput["player"];
                if(playerData.size() < 6)continue;
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
        enemy.velocity = Engine::Point(0, 0);
        return;
    }
    
    // Update path if needed (only if player moved significantly or we don't have a path)
    if (enemy.path.empty() || 
        std::hypot(targetPosition.x - enemy.lastTarget.x, targetPosition.y - enemy.lastTarget.y) > blockSize * 2) {
        enemy.path = FindPath(enemy.position, targetPosition, sender.storedMapState.value(), blockSize);
        enemy.lastTarget = targetPosition;
    }
    
    
    if (!enemy.path.empty()) {
        // Follow the path
        Engine::Point nextWaypoint = enemy.path.front();
        Engine::Point direction = nextWaypoint - enemy.position;
        // std::cerr<<"next "<<nextWaypoint.x<<' '<<nextWaypoint.y<<'\n';
        if (direction.Magnitude() < 5.0f) { // Reached waypoint
            enemy.path.erase(enemy.path.begin());
            if (!enemy.path.empty()) {
                direction = enemy.path.front() - enemy.position;
            }
        }

        if (direction.Magnitude() > 0) {
            direction = direction.Normalize();
        }

        // Update velocity and position
        // std::cerr<<"speed "<< enemy.speed<<'\n';
        enemy.velocity = direction * enemy.speed;
        enemy.position.x += (enemy.velocity.x * deltaTime);
        enemy.position.y += (enemy.velocity.y * deltaTime);
    } else {
        // No path found - don't move through obstacles
        enemy.velocity = Engine::Point(0, 0);
    }
   
    // Boundary check
    int mapWidth = sender.storedMapState.value()["map"]["MapWidth"];
    int mapHeight = sender.storedMapState.value()["map"]["MapHeight"];
    enemy.position.x = std::max(0.0f, std::min(enemy.position.x, static_cast<float>(mapWidth) * blockSize));
    enemy.position.y = std::max(0.0f, std::min(enemy.position.y, static_cast<float>(mapHeight) * blockSize));
    // std::cout<<"enemyid "<<enemy.id<<" nextPosition "<<enemy.position.x<<' '<<enemy.position.y<<' '<<"hp: "<<enemy.hp<<' '<<enemy.alive<<'\n';
    // hit cooldown 
    
}
// void UpdateEnemyInstance(Enemy& enemy, float deltaTime, RenderSender& sender) {
//     enemy.Update(deltaTime);
//     // Find the closest player
//     float minDistance = std::numeric_limits<float>::max();
//     Engine::Point targetPosition(0,0);
//     bool foundPlayer = false;
    
//     {
//         std::lock_guard<std::mutex> lock(sender.clientMutex);
//         for (const auto& ctx : sender.clients) {
//             if (ctx->active && sender.frame.contains(std::to_string(ctx->id))) {
//                 auto playerData = sender.frame[std::to_string(ctx->id)]["player"];
//                 if (playerData.is_array() && playerData.size() >= 2) {
//                     Engine::Point playerPos((float)playerData[0], (float)playerData[1]);
//                     float distance = std::hypot(enemy.position.x - playerPos.x, enemy.position.y - playerPos.y);
//                     if (distance < minDistance) {
//                         minDistance = distance;
//                         targetPosition = playerPos;
//                         foundPlayer = true;
//                     }
//                 }
//             }
//         }
//     }
    
//     if (!foundPlayer) {
//         // No players found, idle
//         enemy.velocity = Engine::Point(0, 0);
//         return;
//     }
    
//     // Calculate direction to player
//     Engine::Point direction = targetPosition - enemy.position;
//     if (direction.Magnitude() > 0) {
//         direction = direction.Normalize();
//     }
    
//     // Update velocity and position
//     enemy.velocity = direction * enemy.speed;
//     enemy.position.x += enemy.velocity.x * deltaTime;
//     enemy.position.y += enemy.velocity.y * deltaTime;
    
//     // Update rotation for facing direction
//     enemy.rotation = std::atan2(direction.y, direction.x) * 180.0f / M_PI;
    
//     // Boundary check
//     enemy.position.x = std::max(0.0f, std::min(enemy.position.x, static_cast<float>(sender.storedMapState.value()["map"]["MapWidth"]) * blockSize));
//     enemy.position.y = std::max(0.0f, std::min(enemy.position.y, static_cast<float>(sender.storedMapState.value()["map"]["MapHeight"]) * blockSize));
//     // hit cooldown
//     enemy.hit_cooldown -= deltaTime;
//     if(enemy.hit_cooldown < 0) enemy.hit_cooldown = 0;
    
// }
void UpdateEnemyHit(Enemy& enemy, float deltaTime, RenderSender& sender, std::vector<HitInformation>& hit_information){
    for(auto it:hit_information) {
        if(enemy.alive&&enemy.hit_cooldown <= 0){
            enemy.Hit(it.damage, sender);
            // std::cerr<<"hit "<<enemy.id<<' '<<" by "<<it.player_id<<'\n';
            if(!enemy.alive)
            for(auto client:sender.clients){
                // std::cerr<<"enemy die "<<enemy.id<<' '<<client->id<<'\n';
                if(client->id == it.player_id && client->lastInput.contains("player") && client->lastInput["player"].size() ==6){
                    client->money += enemy.money;
                }
            }
            enemy.hit_cooldown = 0.05f;
        
        }
        std::cerr<<"hit!! "<<enemy.type<<' '<<enemy.hp<<"\n";
    }
    
}