#ifndef ARMOREDENEMY_HPP
#define ARMOREDENEMY_HPP
#include "Enemy.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/AudioHelper.hpp"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
class ArmoredEnemy : public Enemy {
protected:
    float armor;
    Engine::Sprite* NAND;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> hitSound;
public:
    ArmoredEnemy(float x, float y);
    void Hit(float damage) override;
    ArmoredEnemy(int id, float x, float y);
    void Draw() const override;
    void UpdateFromServer(float x, float y, float rotation, float hp, bool alive, float armor, bool stealth) override;
};
#endif