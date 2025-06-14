#include "OrbitBullet.hpp"
#include "Engine/Point.hpp"
#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Enemy/Enemy.hpp"
#include "UI/Component/Label.hpp"
OrbitBullet::OrbitBullet(float initAngle, float radius, float angularSpeed, Player* centerPlayer, Weapon *parent)
    : Bullet("play/orbitbullet.png", 0, 0, Engine::Point(0, 0), Engine::Point(0, 0), 0, parent),
      angle(initAngle), radius(radius), angularSpeed(angularSpeed), centerPlayer(centerPlayer) {
    // 初始化位置
    Position.x = centerPlayer->Position.x + radius * cos(angle);
    Position.y = centerPlayer->Position.y + radius * sin(angle);
    Size.x = 48 * (1+parent->level), Size.y = 48 * (1+parent->level);
    damage += parent->level * 10;
    angularSpeed += parent->level * 10;
    CollisionRadius = 24 * (1+parent->level);
}

void OrbitBullet::Update(float deltaTime) {
    // 更新角度
    angle += angularSpeed * deltaTime;


    Position.x = centerPlayer->Position.x + radius * cos(angle);
    Position.y = centerPlayer->Position.y + radius * sin(angle);

    Rotation = angle;

    
    Sprite::Update(deltaTime);
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

void OrbitBullet::Draw() const {
    Sprite::Draw();
}