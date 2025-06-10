#define NOMINMAX
#include "Player.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"
#include <algorithm>

Player::Player(float x, float y) : 
    speed(250.0f), 
    health(100), 
    maxHealth(100),
    movingUp(false), 
    movingDown(false), 
    movingLeft(false), 
    movingRight(false),Engine::Sprite("play/Player/mPlayer_ [human].png", x, y,0,0) {
    
    Position = Engine::Point(x, y);
    Size = Engine::Point(128, 128);
    
    SourceH = 32;
    SourceW = 32;

    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    GameClient &sender = game.GetSender();
    sender.output_json["player"] = {x, y};
}


Player::Player(float x, float y, int id):id(id), 
    speed(250.0f), 
    health(100), 
    maxHealth(100),
    movingUp(false), 
    movingDown(false), 
    movingLeft(false), 
    movingRight(false),Engine::Sprite("play/Player/mPlayer_ [human].png", x, y,0,0) {
    
    Position = Engine::Point(x, y);
    Size = Engine::Point(128, 128);
    
    SourceH = 32;
    SourceW = 32;

    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    GameClient &sender = game.GetSender();
    sender.output_json["player"] = {x, y};
}

void Player::Update(float deltaTime) {
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    GameClient &sender = game.GetSender();
    if(game.my_id == id)
    sender.output_json["player"] = {nextPosition.x, nextPosition.y};

}

void Player::UpdateMyPlayer(float deltaTime) {
    
    Engine::Point velocity(0, 0);
    
    if (movingUp) velocity.y -= speed;
    if (movingDown) velocity.y += speed;
    if (movingLeft) velocity.x -= speed;
    if (movingRight) velocity.x += speed;
    

    if (velocity.x != 0 && velocity.y != 0) {
        velocity.x *= 0.707f; // sqrt(2)/2
        velocity.y *= 0.707f;
    }
    

    float newX = Position.x + velocity.x * deltaTime;
    float newY = Position.y + velocity.y * deltaTime;
    

    const int mapWidth = PlayScene::MapWidth * PlayScene::BlockSize;
    const int mapHeight = PlayScene::MapHeight * PlayScene::BlockSize;
    
    newX = std::max(0.0f, std::min(newX, (float)(mapWidth)));
    newY = std::max(0.0f, std::min(newY, (float)(mapHeight)));
    
    nextPosition.x = newX, nextPosition.y = newY;
}

void Player::Draw() const {
    Sprite::Draw();
}
// move by input_json
void Player::OnKeyDown(int keyCode) {
    std::cerr << "Key Down: " << keyCode << std::endl;
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