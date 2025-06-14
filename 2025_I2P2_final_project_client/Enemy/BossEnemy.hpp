#ifndef BOSSENEMY_HPP
#define BOSSENEMY_HPP

#include "Enemy.hpp"

class BossEnemy : public Enemy {
public:
    BossEnemy(int id, float x, float y);
    void UpdateFromServer(float x, float y, float rotation, float hp, bool alive, float armor, bool stealth, int heal) override;
    void Draw() const override;
};

#endif // BOSSENEMY_HPP