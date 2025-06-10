#ifndef CIRCLEWEAPON_HPP
#define CIRCLEWEAPON_HPP
#include "Weapon.hpp"

class CircleWeapon : public Weapon {
public:
    CircleWeapon(float x, float y);
    void CreateBullet() override;
};
#endif   // CIRCLEWEAPON_HPP
