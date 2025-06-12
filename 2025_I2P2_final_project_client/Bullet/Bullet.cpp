#include "Bullet.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/Collider.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IObject.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "Scene/PlayScene.hpp"

PlayScene *Bullet::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}
void Bullet::OnExplode(Enemy *enemy) {
}
Bullet::Bullet(std::string img, float speed, float damage, Engine::Point position, Engine::Point forwardDirection, float rotation, Weapon *parent) : Sprite(img, position.x, position.y), speed(speed), damage(damage), parent(parent) {
    Velocity = forwardDirection.Normalize() * speed;
    Rotation = rotation;
    CollisionRadius = 4;
}
void Bullet::Update(float deltaTime) {
    Sprite::Update(deltaTime);
    PlayScene* scene = getPlayScene();

    // Compute new position
    Engine::Point nextPos = Position + Velocity * deltaTime;

    // Line stepping resolution
    const float step = 4.0f; // step size in pixels
    Engine::Point dir = (nextPos - Position).Normalize();
    float totalDist = (nextPos - Position).Magnitude();
    int steps = std::ceil(totalDist / step);

    for (int i = 1; i <= steps; ++i) {
        Engine::Point checkPos = Position + dir * (step * i);
        if (!scene->isWalkable(checkPos.x, checkPos.y, CollisionRadius)) {
            scene->BulletGroup->RemoveObject(objectIterator);
            std::cerr << "Bullet hit wall mid-path, removed.\n";
            return;
        }
    }

    // Update bullet position
    Position = nextPos;

    // Check out-of-bounds after movement
    if (!Engine::Collider::IsRectOverlap(Position - Size / 2, Position + Size / 2, Engine::Point(0, 0), PlayScene::GetClientSize())) {
        scene->BulletGroup->RemoveObject(objectIterator);
        return;
    }
}