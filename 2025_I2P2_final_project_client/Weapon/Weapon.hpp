#ifndef WEAPON_HPP
#define WEAPON_HPP
#include <allegro5/base.h>
#include <list>
#include <string>

#include "Engine/Sprite.hpp"

class Enemy;
class PlayScene;

class Weapon : public Engine::Sprite {
protected:
    int price;
    float coolDown;
    float reload = 0;
    float rotateRadian = 2 * ALLEGRO_PI;
    PlayScene *getPlayScene();
    // Reference: Design Patterns - Factory Method.
    virtual void CreateBullet() = 0;
    float angle;
    float delta = 0;
    float animation_tick;
public:
    bool aim_front = 1;
    bool Enabled = true;
    bool Preview = false;
    Weapon(std::string imgWeapon, float x, float y, float radius, float coolDown);
    void Update(float deltaTime) override;
    void Draw() const override;
};
#endif   // WEAPON_HPP