#ifndef GUNWEAPON_HPP
#define GUNWEAPON_HPP
#include "Weapon.hpp"

class GunWeapon : public Weapon {
public:
    GunWeapon(float x, float y, int _owner_id, int _level);
    void CreateBullet() override;
    void Update(float deltaTime) override; 
    void Upgrade() override;
};
#endif   // GUNWEAPON_HPP