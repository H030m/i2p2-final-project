#ifndef CIRCLEWEAPON_HPP
#define CIRCLEWEAPON_HPP
#include "Weapon.hpp"
#include "vector"
#include "Bullet/OrbitBullet.hpp"
class CircleWeapon : public Weapon {
private:
    bool created;
    std::vector<OrbitBullet*> oldbullet;
public:
    CircleWeapon(float x, float y, int _owner_id, int _level);
    void CreateBullet() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void Upgrade() override;
};
#endif   // CIRCLEWEAPON_HPP