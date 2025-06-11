#ifndef BOUNCEBULLET_HPP
#define BOUNCEBULLET_HPP
#include "Bullet.hpp"
#include "Weapon/Weapon.hpp"
class Enemy;
class Weapon;
namespace Engine {
    struct Point;
}   // namespace Engine

class BounceBullet : public Bullet {
protected:
    int bounce_time;
    bool bounced;
    float angle;
public:
    explicit BounceBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Weapon *parent);
    void Update(float deltaTime) override;
    void Draw() const override;
};
#endif   // BOUNCEBULLET_HPP
