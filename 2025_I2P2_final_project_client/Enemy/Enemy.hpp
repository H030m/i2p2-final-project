#ifndef ENEMY_HPP
#define ENEMY_HPP
#include <list>
#include <string>
#include <vector>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"

class Bullet;
class PlayScene;
class Turret;

class Enemy : public Engine::Sprite {
protected:
    std::vector<Engine::Point> path;
    int money;
    PlayScene *getPlayScene();
    virtual void OnExplode();
    
public:
    bool tag = 0;
    float hp;
    float speed;
    bool poison = 0;
    float poison_cooldown = 0;
    float reachEndTime;
    int damage = 10;
    Engine::Point HitV = Engine::Point(0.0, 0.0);
    std::list<Turret *> lockedTurrets;
    std::list<Bullet *> lockedBullets;
    Enemy(std::string img, float x, float y, float radius, float speed, float hp, int money);
    virtual void Hit(float damage);
    void UpdatePath(const std::vector<std::vector<int>> &mapDistance);
    void Update(float deltaTime) override;
    void Draw() const override;
};
#endif   // ENEMY_HPP