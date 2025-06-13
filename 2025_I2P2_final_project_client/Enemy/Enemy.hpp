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
    int money;
    PlayScene *getPlayScene();
    virtual void OnExplode();
    
public:
    int id;
    int type;
    bool alive = true;
    bool tag = 0;
    float hp;
    float speed;
    bool poison = 0;
    float poison_cooldown = 0;
    float reachEndTime;
    int damage = 10;
    Engine::Point HitV = Engine::Point(0.0, 0.0);
    Enemy(int id, std::string img, float x, float y, float radius, float speed, float hp, int money);
    virtual void Hit(float damage);
    void Update(float deltaTime) override;
    void Draw() const override;
    virtual void UpdateFromServer(float x, float y, float rotation, float hp, bool alive, float armor, bool stealth);
};
#endif   // ENEMY_HPP