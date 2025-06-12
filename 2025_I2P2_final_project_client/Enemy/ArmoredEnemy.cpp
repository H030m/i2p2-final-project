#include <string>

#include "ArmoredEnemy.hpp"
#include "Scene/PlayScene.hpp"

ArmoredEnemy::ArmoredEnemy(float x, float y)
    : Enemy("play/enemy-1.png", x, y, 10, 10, 10, 10), armor(50) {
    Tint = al_map_rgba(150, 150, 255, 255);
}

void ArmoredEnemy::Hit(float damage) {
    // if (armor > 0) {
    //     float reducedDamage = damage * 0.5f; // 50% damage reduction
    //     armor -= damage; // Armor absorbs full damage
    //     if (armor < 0) armor = 0;
    //     hp -= reducedDamage;
    // } else {
    //     hp -= damage;
    // }
    // if (hp <= 0) {
    //     OnExplode();
    //     getPlayScene()->EarnMoney(money);
    //     getPlayScene()->EnemyGroup->RemoveObject(objectIterator);
    
    //     hp += damage;
    // }
    // Enemy::Hit(damage);
}