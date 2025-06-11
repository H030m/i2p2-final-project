#ifndef BOUNCEWEAPON_HPPAdd
#define BOUNCEWEAPON_HPP
#include "Weapon.hpp"

class BounceWeapon : public Weapon {
public:
    BounceWeapon(float x, float y);
    void CreateBullet() override;
    void Update(float deltaTime) override;
};
#endif   // BOUNCEWEAPON_HPP