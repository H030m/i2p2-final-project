#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Engine/IObject.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/Point.hpp"

class Player : public Engine::IObject {
private:
    Engine::Sprite* sprite;
    bool movingUp, movingDown, movingLeft, movingRight;
    
public:
    // 玩家屬性
    int id;
    int health;
    float speed;
    int maxHealth;
    Engine::Point Position;
    Engine::Point Size;
    
    // 構造函數
    Player(float x, float y);
    ~Player();
    
    // 覆寫基類函數
    void Update(float deltaTime) override;
    void Draw() const override;
    
    // 按鍵處理
    void OnKeyDown(int keyCode);
    void OnKeyUp(int keyCode);
    
    // 遊戲邏輯
    void TakeDamage(int damage);
    bool IsAlive() const;
    void Heal(int amount);
    
    // 碰撞檢測用
    Engine::Point GetCenter() const;
    float GetRadius() const;
};

#endif // PLAYER_HPP