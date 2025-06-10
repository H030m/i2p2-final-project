#ifndef GUNWEAPON_HPP
#define GUNWEAPON_HPP
#include "Weapon.hpp"

class GunWeapon : public Weapon {
public:
    GunWeapon(float x, float y);
    void CreateBullet() override;
};
#endif   // GUNWEAPON_HPP