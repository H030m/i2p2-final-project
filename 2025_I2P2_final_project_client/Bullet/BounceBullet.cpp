#include <allegro5/base.h>
#include <random>
#include <string>

#include "Enemy/Enemy.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "BounceBullet.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/DirtyEffect.hpp"

class Weapon;

BounceBullet::BounceBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Weapon *parent)
     : Bullet("play/bouncebullet.png", 300, 10, position, forwardDirection, rotation, nullptr) {
    bounce_time = 0;
    Size.x = 64, Size.y = 64;
}

void BounceBullet::Update(float deltaTime) {
    if (bounce_time >= 3) {
        Bullet::Update(deltaTime);
        return;
    }

    Engine::Point topLeft = Engine::Point(0, 0);
    Engine::Point bottomRight = PlayScene::GetClientSize();

    bool bounced = false;

    // �w���U�@�V��m
    Engine::Point nextPos = Position + Velocity * deltaTime;

    if (nextPos.x - CollisionRadius < topLeft.x) {
        Velocity.x *= -1;
        nextPos.x = topLeft.x + CollisionRadius;
        bounced = true;
    } else if (nextPos.x + CollisionRadius > bottomRight.x) {
        Velocity.x *= -1;
        nextPos.x = bottomRight.x - CollisionRadius;
        bounced = true;
    }

    if (nextPos.y - CollisionRadius < topLeft.y) {
        Velocity.y *= -1;
        nextPos.y = topLeft.y + CollisionRadius;
        bounced = true;
    } else if (nextPos.y + CollisionRadius > bottomRight.y) {
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

void BounceBullet::Draw() const {
    Sprite::Draw();
}

// if (!Engine::Collider::IsRectOverlap(Position - Size / 2, Position + Size / 2, Engine::Point(0, 0), PlayScene::GetClientSize()))
//         getPlayScene()->BulletGroup->RemoveObject(objectIterator);