#ifndef CIRCLEWEAPON_HPP
#define CIRCLEWEAPON_HPP
#include "Weapon.hpp"

class CircleWeapon : public Weapon {
private:
    bool created;
public:
    CircleWeapon(float x, float y);
    void CreateBullet() override;
    void Update(float deltaTime) override;
    void Draw() const override;
};
#endif   // CIRCLEWEAPON_HPP
