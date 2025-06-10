#include "OrbitBullet.hpp"
#include "Engine/Point.hpp"
#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/DirtyEffect.hpp"

OrbitBullet::OrbitBullet(float initAngle, float radius, float angularSpeed, Player* centerPlayer)
    : Bullet("play/bullet-2.png", 0, 0, Engine::Point(0, 0), Engine::Point(0, 0), 0, nullptr),
      angle(initAngle), radius(radius), angularSpeed(angularSpeed), centerPlayer(centerPlayer) {
    // 初始化位置
    Position.x = centerPlayer->Position.x + radius * cos(angle);
    Position.y = centerPlayer->Position.y + radius * sin(angle);
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
