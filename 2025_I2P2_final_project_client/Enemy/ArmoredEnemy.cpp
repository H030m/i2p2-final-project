#include <string>

#include "ArmoredEnemy.hpp"
#include "Scene/PlayScene.hpp"

ArmoredEnemy::ArmoredEnemy(std::string img, float x, float y, float radius, float speed, float hp, int money, float armor)
    : Enemy(img, x, y, radius, speed, hp, money), armor(armor) {
    Tint = al_map_rgba(150, 150, 255, 255);
}

void ArmoredEnemy::Hit(float damage) {
    if (armor > 0) {
        float reducedDamage = damage * 0.5f; // 50% damage reduction
        armor -= damage; // Armor absorbs full damage
        if (armor < 0) armor = 0;
        hp -= reducedDamage;
    } else {
        hp -= damage;
    }
    if (hp <= 0) {
        OnExplode();
        getPlayScene()->EarnMoney(money);
        getPlayScene()->EnemyGroup->RemoveObject(objectIterator);
    }
}