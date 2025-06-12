#ifndef BOUNCEWEAPON_HPP
#define BOUNCEWEAPON_HPP
#include "Weapon.hpp"

class BounceWeapon : public Weapon {
public:
    BounceWeapon(float x, float y, int _owner_id);
    BounceWeapon(float x, float y, int _owner_id, int _level);
    void CreateBullet() override;
    void Update(float deltaTime) override;
};
#endif   // BOUNCEWEAPON_HPP