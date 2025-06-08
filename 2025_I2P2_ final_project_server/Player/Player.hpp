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
    // ���a�ݩ�
    int id;
    int health;
    float speed;
    int maxHealth;
    Engine::Point Position;
    Engine::Point Size;
    
    // �c�y���
    Player(float x, float y);
    ~Player();
    
    // �мg�������
    void Update(float deltaTime) override;
    void Draw() const override;
    
    // ����B�z
    void OnKeyDown(int keyCode);
    void OnKeyUp(int keyCode);
    
    // �C���޿�
    void TakeDamage(int damage);
    bool IsAlive() const;
    void Heal(int amount);
    
    // �I���˴���
    Engine::Point GetCenter() const;
    float GetRadius() const;
};

#endif // PLAYER_HPP