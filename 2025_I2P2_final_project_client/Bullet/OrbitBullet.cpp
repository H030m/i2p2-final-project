#include "OrbitBullet.hpp"
#include "Engine/Point.hpp"
#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/DirtyEffect.hpp"

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

    // 根據 player 位置重新設定位置
    Position.x = centerPlayer->Position.x + radius * cos(angle);
    Position.y = centerPlayer->Position.y + radius * sin(angle);

    Rotation = angle;

    // 不需要 Bullet 的移動邏輯，因此不呼叫 Bullet::Update
    Sprite::Update(deltaTime);
}

void OrbitBullet::Draw() const {
    Sprite::Draw();
}