#include <allegro5/base.h>
#include <random>
#include <string>
#include "UI/Component/Label.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "BounceBullet.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/DirtyEffect.hpp"

class Weapon;

BounceBullet::BounceBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Weapon *parent)
    : Bullet("play/bouncebullet.png", 300 + parent->level * 100, 10 + parent->level * 10, position, forwardDirection, rotation, parent) {
    bounce_time = 0;
    Size.x = 48 * (1+parent->level*0.5), Size.y = 48 * (1+parent->level*0.5);
    CollisionRadius = 24 * (1+parent->level*0.5);
}

void BounceBullet::Update(float deltaTime) {
    if (bounce_time >= 3 + parent->level) {
        Bullet::Update(deltaTime);
        return;
    }

    Engine::Point topLeft = Engine::Point(0, 0);
    Engine::Point bottomRight = PlayScene::GetClientSize();
    bool bounced = false;

    // Predict next position.
    Engine::Point nextPos = Position + Velocity * deltaTime;

    // Check if the current scene is PlayScene.
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    if (game.CurrentScene == "play") {
        PlayScene* scene = dynamic_cast<PlayScene*>(game.GetActiveScene());
        if (scene) {
            // Check horizontal obstacle collision.
            if (!scene->isWalkable(nextPos.x, Position.y, CollisionRadius)) {
                Velocity.x *= -1;
                nextPos.x = Position.x; // Prevent penetration
                bounced = true;
            }
            // Check vertical obstacle collision.
            if (!scene->isWalkable(Position.x, nextPos.y, CollisionRadius)) { {
                Velocity.y *= -1;
                nextPos.y = Position.y;
                bounced = true;
            }
        }
    }

    // Check left/right boundary collision.
    if (nextPos.x - CollisionRadius/2 < topLeft.x) {
        Velocity.x *= -1;
        nextPos.x = topLeft.x + CollisionRadius;
        bounced = true;
    } else if (nextPos.x + CollisionRadius/2 > bottomRight.x) {
        Velocity.x *= -1;
        nextPos.x = bottomRight.x - CollisionRadius;
        bounced = true;
    }

    // Check top/bottom boundary collision.
     if (nextPos.y - CollisionRadius/2 < topLeft.y) {
        Velocity.y *= -1;
        nextPos.y = topLeft.y + CollisionRadius;
        bounced = true;
    } else if (nextPos.y + CollisionRadius/2 > bottomRight.y) {
        Velocity.y *= -1;
        nextPos.y = bottomRight.y - CollisionRadius;
        bounced = true;
    }

    if (bounced) {
        bounce_time++;
    }

    Position = nextPos;
    Rotation = atan2(Velocity.y, Velocity.x) + ALLEGRO_PI / 2;

    Sprite::Update(deltaTime);
    }
    PlayScene* scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
    for (auto& enemyObj : scene->EnemyGroup->GetObjects()) {
        Enemy* enemy = dynamic_cast<Enemy*>(enemyObj);
        if (enemy && enemy->alive) {
            Engine::Point diff = enemy->Position - Position;
            float distance = diff.Magnitude();
            if (distance <= enemy->CollisionRadius + CollisionRadius) {
                Engine::GameEngine &game = Engine::GameEngine::GetInstance();
                if(enemy->type == 0 && game.getscore_cooldown <= 0){
                    scene->player_dict[scene->my_id]->Heal(2);
                    game.getscore_cooldown = 0.1f;
                    game.DYYscore += 2;
                    scene->scoreGainLabel->Text = "+0.2";
                    scene->scoreGainDisplayTime = 0.2f;
                }
                enemy->Hit(damage); // Apply damage to enemy
                OnExplode(enemy);   // Trigger hit effect
                // return; // Stop checking after hitting an enemy
            }
        }
    }
}

void BounceBullet::Draw() const {
    Sprite::Draw();
}

// if (!Engine::Collider::IsRectOverlap(Position - Size / 2, Position + Size / 2, Engine::Point(0, 0), PlayScene::GetClientSize()))
//         getPlayScene()->BulletGroup->RemoveObject(objectIterator);