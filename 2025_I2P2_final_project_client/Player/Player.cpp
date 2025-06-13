#define NOMINMAX
#include "Player.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"
#include <algorithm>
#include "Weapon/Weapon.hpp"
#include "Engine/LOG.hpp"
#include <vector>
#include "Enemy/Enemy.hpp"
class Weapon;
Player::Player(float x, float y) : 
    speed(250.0f), 
    health(100), 
    maxHealth(100),
    movingUp(false), 
    movingDown(false), 
    movingLeft(false), 
    movingRight(false),Engine::Sprite("play/Player/mPlayer_ [human].png", x, y, 0, 0) {
    
    Position = Engine::Point(x, y);
    Size = Engine::Point(128, 128);
    
    SourceH = 32;
    SourceW = 32;

    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    GameClient &sender = game.GetSender();

}

Player::Player(float x, float y, int id):id(id), 
    speed(250.0f), 
    health(100), 
    maxHealth(100),
    movingUp(false), 
    movingDown(false), 
    movingLeft(false), 
    movingRight(false),Engine::Sprite("play/Player/mPlayer_ [human].png", x, y,0,0) {
    
    Position = Engine::Point(x, y);
    Size = Engine::Point(128, 128);
    
    SourceH = 32;
    SourceW = 32;

    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    GameClient &sender = game.GetSender();
}

Player::Player(float x, float y, int id, int MapWidth, int MapHeight):id(id), 
    speed(250.0f), 
    health(100), 
    maxHealth(100),
    movingUp(false), 
    movingDown(false), 
    movingLeft(false), 
    movingRight(false),Engine::Sprite("play/Player/mPlayer_ [human].png", x, y,0,0),
    MapWidth(MapWidth),MapHeight(MapHeight) {
    
    Position = Engine::Point(x, y);
    Size = Engine::Point(128, 128);
    
    SourceH = 32;
    SourceW = 32;

    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    GameClient &sender = game.GetSender();
    CollisionRadius = 55;
     std::cerr<<"new Player at "<<x<<" "<<y<<'\n';
    gold = 69;
}

void Player::Update(float deltaTime) {
    
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    GameClient &sender = game.GetSender();

    

    // animation 
    SourceW = 32; SourceH = 32;
    animation_tick += deltaTime;

    switch(status) {
        case(PLAYER_WALK) :
            SourceY = 32 * 6;
            if (animation_tick >= 0.1f) {
                animation_tick = 0;
                // add 32
                SourceX += SourceW;
            }

            // totally n pitcure (0 -> 32 -> 64 -> ...)
            if (SourceX >= 256) // 32 * n 
                SourceX = 0;
            break;

        case(PLAYER_IDLE) :
            SourceY = 32 * 1;
            if (animation_tick >= 0.2f) {
                animation_tick = 0;
                SourceX += SourceW;
            }

            if (SourceX >= 128) 
                SourceX = 0;
            break;
    }
    //
    damageCooldown -= deltaTime;
    if(damageCooldown < 0) damageCooldown = 0;
}

void Player::UpdateMyPlayer(float deltaTime) {
    //Status
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    if ((movingDown || movingUp || movingLeft || movingRight) && id == game.my_id) {
        status = PLAYER_WALK;
        if (movingLeft && !movingRight) Flip = 1;
        else if (movingRight && !movingLeft) Flip = 0;
    }
    else {
        status = PLAYER_IDLE;
    }
    
    //next position
    Engine::Point velocity(0, 0);
    if (movingUp) velocity.y -= speed;
    if (movingDown) velocity.y += speed;
    if (movingLeft) velocity.x -= speed;
    if (movingRight) velocity.x += speed;

    if (velocity.x != 0 && velocity.y != 0) {
        velocity.x *= 0.707f;
        velocity.y *= 0.707f;
    }

    float newX = Position.x + velocity.x * deltaTime;
    float newY = Position.y + velocity.y * deltaTime;

    const int mapWidth = MapWidth * BlockSize;
    const int mapHeight = MapHeight * BlockSize;
    newX = std::max(0.0f, std::min(newX, (float)(mapWidth)));
    newY = std::max(0.0f, std::min(newY, (float)(mapHeight)));

 
    int errorX = 0, errorY = 16;
    if (game.CurrentScene == "play") {
        PlayScene* scene = dynamic_cast<PlayScene*>(game.GetActiveScene());

        if (scene&&id==scene->my_id) {
            if (!spawned) {
                if (!scene->isWalkable(Position.x + errorX, Position.y  + errorY , CollisionRadius)) {
                    // Try to find nearest walkable point (only once)
                    const int maxSearchRadius = 5;
                    bool found = false;
                    for (int r = 1; r <= maxSearchRadius && !found; ++r) {
                        for (int dx = -r; dx <= r && !found; ++dx) {
                            for (int dy = -r; dy <= r && !found; ++dy) {
                                if (abs(dx) != r && abs(dy) != r) continue;
                                int tx = Position.x + dx * BlockSize;
                                int ty = Position.y + dy * BlockSize;
                                if (tx < 0 || ty < 0 || tx >= mapWidth || ty >= mapHeight) continue;
                                if (scene->isWalkable(tx + errorX, ty + errorY, CollisionRadius)) {
                                    Position.x = tx;
                                    Position.y = ty;
                                    found = true;
                                    std::cerr << "[Spawn Fix] Player moved to walkable tile.\n";
                                }
                            }
                        }
                    }
                    if (!found) std::cerr << "[Spawn Fix] No nearby walkable tile found.\n";
                }
                spawned = true;
            }

            // normal movement blocked
            // if (!scene->isWalkable(newX + errorX, newY + errorY, CollisionRadius))
            //     return;
            float finalX = Position.x; float finalY = Position.y;

            if (velocity.x != 0) {
                if (scene->isWalkable(newX + errorX, Position.y + errorY, CollisionRadius)) {
                    finalX = newX;
                }
            }

            if (velocity.y != 0) {
                if (scene->isWalkable(finalX + errorX, newY + errorY, CollisionRadius)) {
                    finalY = newY;
                }
            }
            Position.x = finalX;
            Position.y = finalY;
        }
        else {
            Position.x = newX;
            Position.y = newY;
        }
    }else {
        Position.x = newX;
        Position.y = newY;
    }

    // interaction with enemy
    if(Engine::GameEngine::GetInstance().CurrentScene == "play"){
        PlayScene* scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
    
        if(damageCooldown < 0)damageCooldown = 0;
        // Check for enemy collisions first
        for (auto& enemyObj : scene->EnemyGroup->GetObjects()) {
            Enemy* enemy = dynamic_cast<Enemy*>(enemyObj);
            if (enemy) {
                Engine::Point diff = enemy->Position - Position;
                float distance = diff.Magnitude();
                if (distance <= enemy->CollisionRadius + CollisionRadius) {
                    if(damageCooldown <= 0)
                     Engine::LOG(Engine::INFO) << "Player is hit by enemy at (" << enemy->id << ")";
                    TakeDamage(enemy->damage); // Apply damage to Player
                    // std::cerr<<"oh no health "<<health<<'\n';
                    // Engine::LOG(Engine::INFO) << "Player is hit by enemy at (" << enemy->id << ")";
                }
            }
        }
    }
    
}

void Player::Draw() const {
    Sprite::Draw();
}
// move by input_json
void Player::OnKeyDown(int keyCode) {
    std::cerr << "Key Down: " << keyCode << std::endl;
    switch (keyCode) {
        case ALLEGRO_KEY_UP:
        case ALLEGRO_KEY_W:
            movingUp = true;
            break;
        case ALLEGRO_KEY_DOWN:
        case ALLEGRO_KEY_S:
            movingDown = true;
            break;
        case ALLEGRO_KEY_LEFT:
        case ALLEGRO_KEY_A:
            movingLeft = true;
            break;
        case ALLEGRO_KEY_RIGHT:
        case ALLEGRO_KEY_D:
            movingRight = true;
            break;
        case ALLEGRO_KEY_Q:
           gold += 100;
           break;
    }
}

void Player::OnKeyUp(int keyCode) {
    switch (keyCode) {
        case ALLEGRO_KEY_UP:
        case ALLEGRO_KEY_W:
            movingUp = false;
            break;
        case ALLEGRO_KEY_DOWN:
        case ALLEGRO_KEY_S:
            movingDown = false;
            break;
        case ALLEGRO_KEY_LEFT:
        case ALLEGRO_KEY_A:
            movingLeft = false;
            break;
        case ALLEGRO_KEY_RIGHT:
        case ALLEGRO_KEY_D:
            movingRight = false;
            break;
    }
}

void Player::TakeDamage(int damage) {
    if(damageCooldown > 0)return;
    health -= damage;
    if (health < 0) health = 0;
    damageCooldown = 0.3f;
    
}

bool Player::IsAlive() const {
    return health > 0;
}

void Player::Heal(int amount) {
    health += amount;
    if (health > maxHealth) health = maxHealth;
}

Engine::Point Player::GetCenter() const {
    return Engine::Point(Position.x + Size.x / 2, Position.y + Size.y / 2);
}

float Player::GetRadius() const {
    return Size.x / 2.0f;
}