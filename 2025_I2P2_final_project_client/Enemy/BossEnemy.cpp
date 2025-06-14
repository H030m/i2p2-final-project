#include "BossEnemy.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Player/Player.hpp"
#include "Engine/LOG.hpp"
#include "Engine/AudioHelper.hpp"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <iostream>

BossEnemy::BossEnemy(int id, float x, float y)
    : Enemy(id, "play/DYY.png", x, y, 64, 0, 500, 0) {
    type = 0;
    Tint = al_map_rgba(255, 255, 255, 255);
    timeSinceLastDamage = 0.0f;
    Size.x = 128;
    Size.y = 128;
    damageInterval = 10.0f;
    damageAmount = 50.0f;

    attackSound = AudioHelper::PlaySample("108.wav", false, 2.5f); // 108
    al_set_sample_instance_playmode(attackSound.get(), ALLEGRO_PLAYMODE_ONCE);

    hitSound = AudioHelper::PlaySample("add0.2.wav", false, 5.0f); // +0.2
    al_set_sample_instance_playmode(hitSound.get(), ALLEGRO_PLAYMODE_ONCE);
}

void BossEnemy::Update(float deltaTime) {
    Enemy::Update(deltaTime);
    
    if (!alive) return;

    // Update damage timer
    timeSinceLastDamage += deltaTime;
    
    // Deal damage to player when in range
    if (timeSinceLastDamage >= damageInterval) {
        timeSinceLastDamage = 0.0f;
        
        PlayScene* scene = getPlayScene();
        if (!scene || !scene->player_dict.count(scene->my_id)) return;
        
        Player* player = scene->player_dict[scene->my_id];
        float distance = (player->Position - Position).Magnitude();
        
        if (distance <= attackRadius + player->CollisionRadius) {
            player->TakeDamage(damageAmount);
            Engine::LOG(Engine::INFO) << "Boss damaged player!";

            if (attackSound) {
                al_stop_sample_instance(attackSound.get());
                al_play_sample_instance(attackSound.get());
            }
            
            // Start new attack animation
            AttackAnimation anim;
            anim.radius = 0;
            anim.maxRadius = attackRadius;
            anim.progress = 0;
            anim.active = true;
            attackAnimations.push_back(anim);
        }
    }
    
    // Update active attack animations
    for (auto& anim : attackAnimations) {
        if (anim.active) {
            anim.progress += deltaTime / animationDuration;
            anim.radius = anim.maxRadius * anim.progress;
            
            if (anim.progress >= 1.0f) {
                anim.active = false;
            }
        }
    }
    
    // Remove completed animations
    attackAnimations.erase(
        std::remove_if(attackAnimations.begin(), attackAnimations.end(),
            [](const AttackAnimation& a) { return !a.active; }),
        attackAnimations.end());
}

void BossEnemy::UpdateFromServer(float x, float y, float rotation, float hp, bool alive, float armor, bool stealth) {
    bool wasHit = (this->hp > hp); // Check if HP decreased
    
    Enemy::UpdateFromServer(x, y, rotation, hp, alive, armor, stealth);
    
    // Play hit sound if the boss was damaged
    if (wasHit && hitSound && !al_get_sample_instance_playing(hitSound.get())) {
        al_play_sample_instance(hitSound.get());
    }
}

void BossEnemy::Draw() const {
    if (alive) {
        Enemy::Draw();
        
        // Draw attack animations
        for (const auto& anim : attackAnimations) {
            if (anim.active) {
                // Calculate alpha based on progress (fade out)
                float alpha = 255 * (1.0f - anim.progress);
                ALLEGRO_COLOR color = al_map_rgba(255, 0, 0, alpha);
                
                // Draw expanding circle
                al_draw_circle(
                    Position.x, Position.y, 
                    anim.radius, 
                    color, 
                    4.0f * (1.0f - anim.progress) // Thinner as it expands
                );
            }
        }
        
        if (PlayScene::DebugMode) {
            // Draw attack radius
            al_draw_circle(Position.x, Position.y, attackRadius, al_map_rgba(255, 0, 0, 50), 1);
        }
    }
}