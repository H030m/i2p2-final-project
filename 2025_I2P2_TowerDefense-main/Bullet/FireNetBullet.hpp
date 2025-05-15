#ifndef FIRENETBULLET_HPP
#define FIRENETBULLET_HPP
#include "Bullet.hpp"
// cause poison and can earn money from enemy
class Enemy;
class Turret;
namespace Engine {
    struct Point;
}   // namespace Engine

class FireNetBullet : public Bullet {
public:
    explicit FireNetBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret *parent);
    void OnExplode(Enemy *enemy) override;
};
#endif   // LASERBULLET_HPP
