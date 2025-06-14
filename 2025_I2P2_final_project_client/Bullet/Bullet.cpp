#include "Bullet.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/Collider.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IObject.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "UI/Component/Label.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/LOG.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include <random>
PlayScene *Bullet::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}
void Bullet::OnExplode(Enemy *enemy) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
    getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-1.png", dist(rng), enemy->Position.x, enemy->Position.y));
}

Bullet::Bullet(std::string img, float speed, float damage, Engine::Point position, Engine::Point forwardDirection, float rotation, Weapon *parent)
    : Sprite(img, position.x, position.y), speed(speed), damage(damage), parent(parent) {
    Velocity = forwardDirection.Normalize() * speed;
    Rotation = rotation;
    CollisionRadius = 4;
}
void Bullet::Update(float deltaTime) {
    Sprite::Update(deltaTime);
    PlayScene* scene = getPlayScene();

    // Check for enemy collisions first
    bool remove = false;
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
                Engine::LOG(Engine::INFO) << "Bullet hit enemy at (" << enemy->Position.x << ", " << enemy->Position.y << ")";
                // return; // Stop checking after hitting an enemy
                remove = true;
            }
            
        }
    }
    if(remove){
        scene->BulletGroup->RemoveObject(objectIterator);
        return;
    }

    // Compute new position
    Engine::Point nextPos = Position + Velocity * deltaTime;

    // Line stepping resolution for wall collision
    const float step = 4.0f; // Step size in pixels
    Engine::Point dir = (nextPos - Position).Normalize();
    float totalDist = (nextPos - Position).Magnitude();
    int steps = std::ceil(totalDist / step);

    for (int i = 1; i <= steps; ++i) {
        Engine::Point checkPos = Position + dir * (step * i);
        if (!scene->isWalkable(checkPos.x, checkPos.y, CollisionRadius)) {
            scene->BulletGroup->RemoveObject(objectIterator);
             Engine::LOG(Engine::INFO) << "Bullet hit wall at (" << checkPos.x << ", " << checkPos.y << ")";
            return;
        }
    }

    // Update bullet position
    Position = nextPos;

    // Check out-of-bounds after movement
    if (!Engine::Collider::IsRectOverlap(Position - Size / 2, Position + Size / 2, Engine::Point(0, 0), PlayScene::GetClientSize())) {
        scene->BulletGroup->RemoveObject(objectIterator);
        Engine::LOG(Engine::INFO) << "Bullet out of bounds at (" << Position.x << ", " << Position.y << ")";
        return;
    }
}