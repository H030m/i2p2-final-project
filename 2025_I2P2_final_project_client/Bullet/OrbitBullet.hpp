#ifndef ORBITBULLET_HPP
#define ORBITBULLET_HPP

#include "Bullet.hpp"
#include "Engine/Point.hpp"

class Player;

class OrbitBullet : public Bullet {
protected:
    float angle;
    float radius;
    float angularSpeed;
    Player* centerPlayer;

public:
    OrbitBullet(float initAngle, float radius, float angularSpeed, Player* centerPlayer);
    void Update(float deltaTime) override;
    void Draw() const override;
};
#endif //ORBITBULLET_HPP