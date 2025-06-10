
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Engine/IObject.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/Point.hpp"

class Player : public Engine::Sprite {
private:
    bool movingUp, movingDown, movingLeft, movingRight;
    
public:
    int id;
    int health;
    float speed;
    int maxHealth;


    Player(float x, float y);

    void Update(float deltaTime) override;
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
