#include <string>
#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include "ArmoredEnemy.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Connect/Client.hpp"
#include "Engine/AudioHelper.hpp"
#include <allegro5/allegro_audio.h>
ArmoredEnemy::ArmoredEnemy(int id, float x, float y)
    : Enemy(id, "play/MWG.png", x, y, 10, 10, 10, 10) {
    armor = 50;
    type = 1;
    Tint = al_map_rgba(150, 150, 255, 255);
    Size.x = 32;
    Size.y = 32;
    NAND = new Sprite(
        "play/NAND.png",
        0,0
    );
    NAND->Size.x = 64;
    NAND->Size.y = 41;
    NAND->Visible = true;

    hitSound = AudioHelper::PlaySample("nand.wav", false, true);
    al_set_sample_instance_playmode(hitSound.get(), ALLEGRO_PLAYMODE_ONCE);
    al_set_sample_instance_gain(hitSound.get(), 1.5f);
}
void ArmoredEnemy::Draw() const {
    if (alive) {
        Enemy::Draw();
    }
    NAND->Position = Position;
    if(NAND->Visible)NAND->Draw();
    else return;
    // health bar settings
    const float barWidth = 50.0f;
    const float barHeight = 6.0f;
    const float barOffsetY = -30.0f;  
    const int maxHealth = max_hp;
    
    // health bar position
    float barX = NAND->Position.x - barWidth / 2;
    float barY = NAND->Position.y + barOffsetY;
    
    // health bar percent
    float healthPercent = static_cast<float>(armor) / static_cast<float>(50);
    healthPercent = std::max(0.0f, std::min(1.0f, healthPercent));
    
    // health bar background (black)
    al_draw_filled_rectangle(
        barX - 1, barY - 1,
        barX + barWidth + 1, barY + barHeight + 1,
        al_map_rgb(40, 40, 40)
    );
    
    // change health bar color base on percent
    ALLEGRO_COLOR healthColor;
    if (healthPercent > 0.6f) {
        healthColor = al_map_rgb(0, 255, 0);      // green
    } else if (healthPercent > 0.3f) {
        healthColor = al_map_rgb(255, 255, 0);    // yellow
    } else {
        healthColor = al_map_rgb(255, 0, 0);      // red
    }
    
    // draw health bar
    if (healthPercent > 0) {
        al_draw_filled_rectangle(
            barX + 1, barY + 1,
            barX + (barWidth - 2) * healthPercent, barY + barHeight - 1,
            healthColor
        );
    }
    
}
void ArmoredEnemy::Hit(float damage) {
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    GameClient &sender = game.sender;
    sender.output_json["Hit"].push_back({
        {"id",id},
        {"Damage",damage},
        {"HitVx",HitV.x},
        {"HitVy",HitV.y}
    });

    if (armor > 0 && hitSound && !al_get_sample_instance_playing(hitSound.get())) {
        al_play_sample_instance(hitSound.get());
    }
}
void ArmoredEnemy::UpdateFromServer(float x, float y, float rotation, float hp, bool alive, float armor, bool stealth) {
    this->armor = armor;
    if(armor <= 0){
        NAND->Visible = false;
    }else NAND->Visible = true;
    Enemy::UpdateFromServer(x, y, rotation, hp, alive, armor, stealth);
    
}