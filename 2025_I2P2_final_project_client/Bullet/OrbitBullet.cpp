#include "OrbitBullet.hpp"
#include "Engine/Point.hpp"
#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/DirtyEffect.hpp"

OrbitBullet::OrbitBullet(float initAngle, float radius, float angularSpeed, Player* centerPlayer, Weapon *parent)
    : Bullet("play/orbitbullet.png", 0, 0, Engine::Point(0, 0), Engine::Point(0, 0), 0, parent),
      angle(initAngle), radius(radius), angularSpeed(angularSpeed), centerPlayer(centerPlayer) {
    // ��l�Ʀ�m
    Position.x = centerPlayer->Position.x + radius * cos(angle);
    Position.y = centerPlayer->Position.y + radius * sin(angle);
    Size.x = 48 * (1+parent->level), Size.y = 48 * (1+parent->level);
    damage += parent->level * 10;
    angularSpeed += parent->level * 10;
    CollisionRadius = 24 * (1+parent->level);
}

void OrbitBullet::Update(float deltaTime) {
    // ��s����
    angle += angularSpeed * deltaTime;

    // �ھ� player ��m���s�]�w��m
    Position.x = centerPlayer->Position.x + radius * cos(angle);
    Position.y = centerPlayer->Position.y + radius * sin(angle);

    Rotation = angle;

    // ���ݭn Bullet �������޿�A�]�����I�s Bullet::Update
    Sprite::Update(deltaTime);
}

void OrbitBullet::Draw() const {
    Sprite::Draw();
}