#define NOMINMAX
#include "Player.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"
#include <algorithm>

Player::Player(float x, float y) : 
    id(sprite->id),
    speed(250.0f), 
    health(100), 
    maxHealth(100),
    movingUp(false), 
    movingDown(false), 
    movingLeft(false), 
    movingRight(false) {
    
    Position = Engine::Point(x, y);
    Size = Engine::Point(32, 32);
    
    // 載入玩家圖片 - 你需要準備一個玩家圖片
    sprite = new Engine::Sprite("play/bullet-7.png", x, y);
    sprite->Size = Size;
}

Player::~Player() {
    delete sprite;
}

void Player::Update(float deltaTime) {
    // 處理移動
    Engine::Point velocity(0, 0);
    
    if (movingUp) velocity.y -= speed;
    if (movingDown) velocity.y += speed;
    if (movingLeft) velocity.x -= speed;
    if (movingRight) velocity.x += speed;
    
    // 正規化對角線移動速度
    if (velocity.x != 0 && velocity.y != 0) {
        velocity.x *= 0.707f; // sqrt(2)/2
        velocity.y *= 0.707f;
    }
    
    // 計算新位置
    float newX = Position.x + velocity.x * deltaTime;
    float newY = Position.y + velocity.y * deltaTime;
    
    // 邊界檢查
    const int mapWidth = PlayScene::MapWidth * PlayScene::BlockSize;
    const int mapHeight = PlayScene::MapHeight * PlayScene::BlockSize;
    
    newX = std::max(0.0f, std::min(newX, (float)(mapWidth - Size.x)));
    newY = std::max(0.0f, std::min(newY, (float)(mapHeight - Size.y)));
    
    // 更新位置
    Position.x = newX;
    Position.y = newY;
    
    // 更新精靈位置
    sprite->Position = Position;
    sprite->Update(deltaTime);
}

void Player::Draw() const {
    sprite->Draw();
}

void Player::OnKeyDown(int keyCode) {
    switch (keyCode) {
        case ALLEGRO_KEY_UP:
        case ALLEGRO_KEY_W:
            movingUp = true;
            break;
        case ALLEGRO_KEY_DOWN:
        case ALLEGRO_KEY_S:
            movingDown = true;
            break;
        case ALLEGRO_KEY_LEFT:
        case ALLEGRO_KEY_A:
            movingLeft = true;
            break;
        case ALLEGRO_KEY_RIGHT:
        case ALLEGRO_KEY_D:
            movingRight = true;
            break;
    }
}

void Player::OnKeyUp(int keyCode) {
    switch (keyCode) {
        case ALLEGRO_KEY_UP:
        case ALLEGRO_KEY_W:
            movingUp = false;
            break;
        case ALLEGRO_KEY_DOWN:
        case ALLEGRO_KEY_S:
            movingDown = false;
            break;
        case ALLEGRO_KEY_LEFT:
        case ALLEGRO_KEY_A:
            movingLeft = false;
            break;
        case ALLEGRO_KEY_RIGHT:
        case ALLEGRO_KEY_D:
            movingRight = false;
            break;
    }
}

void Player::TakeDamage(int damage) {
    health -= damage;
    if (health < 0) health = 0;
}

bool Player::IsAlive() const {
    return health > 0;
}

void Player::Heal(int amount) {
    health += amount;
    if (health > maxHealth) health = maxHealth;
}

Engine::Point Player::GetCenter() const {
    return Engine::Point(Position.x + Size.x / 2, Position.y + Size.y / 2);
}

float Player::GetRadius() const {
    return Size.x / 2.0f;
}