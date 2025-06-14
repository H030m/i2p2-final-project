#ifndef STEALTHENEMY_HPP
#define STEALTHENEMY_HPP
#include "Enemy.hpp"
// #include "Connect/RenderSender.hpp"
#include <nlohmann/json.hpp>

class StealthEnemy : public Enemy {
private:
    bool stealth;
    float stealthCooldown;
    static const float initStealth;
    static const float initRadius;
    static const float initSpeed;
    static const float initHP;
    static const float initDamage;
    static const float initMoney;
public:
    StealthEnemy(int id, Engine::Point position, Engine::Point spawn);
    void Update(float deltaTime, RenderSender& sender) override;
    void Revive() override;
    void Hit(float damage, RenderSender& sender) override;
    nlohmann::json Serialize() const override;
};
#endif