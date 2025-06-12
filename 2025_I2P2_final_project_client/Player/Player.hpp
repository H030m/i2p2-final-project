
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Engine/IObject.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/Point.hpp"
#include "Weapon/Weapon.hpp"
#include <vector>

class Weapon;

typedef enum Player_Status {
    PLAYER_IDLE,
    PLAYER_WALK,
    PLAYER_ATTACK,
    PLAYER_DEAD
} Player_Status;

class Player : public Engine::Sprite {
private:
    bool movingUp, movingDown, movingLeft, movingRight;
    int MapWidth,BlockSize = 64,MapHeight;
    bool spawned = false;
    
    
    public:
    int id;
    int health;
    float speed;
    int maxHealth;
    Engine::Point nextPosition;
    float animation_tick;
    Player_Status status;
    std::vector<Weapon*> Weapon_hold;
    std::vector<Weapon*> Weapon_owned;
    Player(float x, float y);
    Player(float x, float y, int id);
    Player(float x, float y, int id, int MapWidth, int MapHeight);
    void Update(float deltaTime) override;
    void UpdateMyPlayer(float deltaTime);
    void Draw() const override;
    

    void OnKeyDown(int keyCode);
    void OnKeyUp(int keyCode);
    
    void TakeDamage(int damage);
    bool IsAlive() const;
    void Heal(int amount);
    
    Engine::Point GetCenter() const;
    float GetRadius() const;
};

#endif // PLAYER_HPP
