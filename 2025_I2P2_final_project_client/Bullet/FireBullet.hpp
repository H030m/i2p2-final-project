#ifndef FIREBULLET_HPP
#define FIREBULLET_HPP
#include "Bullet.hpp"
#include "Weapon/Weapon.hpp"
class Enemy;
class Weapon;
namespace Engine {
    struct Point;
}   // namespace Engine

class FireBullet : public Bullet {
public:
    explicit FireBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Weapon *parent);
    void OnExplode(Enemy *enemy) override;
};
#endif   // FIREBULLET_HPP
