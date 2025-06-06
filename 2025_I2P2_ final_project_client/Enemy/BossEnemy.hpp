#ifndef TUNGSHUR_HPP
#define TUNGSHUR_HPP
#include "Enemy.hpp"
#include "Engine/Sprite.hpp"

class BossEnemy : public Enemy {
private:
    float targetRotation;
    float skill_time;
    
public:
    BossEnemy(int x, int y);
    void Draw() const override;
    void Update(float deltaTime) override;
};
#endif   // TANKENEMY_HPP
