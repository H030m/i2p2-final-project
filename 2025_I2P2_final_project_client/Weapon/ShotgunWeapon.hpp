#ifndef SHOTGUNWEAPON_HPP
#define SHOTGUNWEAPON_HPP
#include "Weapon.hpp"

class ShotgunWeapon : public Weapon {
public:
    ShotgunWeapon(float x, float y);
    void CreateBullet() override;
    void Update(float deltaTime) override;
};
#endif   // SHOTGUNWEAPON_HPP