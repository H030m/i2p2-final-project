#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Bullet/OrbitBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "CircleWeapon.hpp"
#include "Weapon.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"
CircleWeapon::CircleWeapon(float x, float y, int _owner_id, int _level)
    : Weapon("play/turret-1.png", x, y, 200, 0.1) {
    // Move center downward, since we the turret head is slightly biased upward.
    // Anchor.y += 8.0f / GetBitmapHeight();
    created = false;
    owner_id =  _owner_id;
    type = 3;
    level = _level;
}
void CircleWeapon::CreateBullet() {
    // Change bullet position to the front of the gun barrel.
    Player* player = getPlayScene()->player_dict[getPlayScene()->my_id];
    int bulletCount = 2 + (level - 1);

    for (int i = 0; i < bulletCount; i++) {
        float angle = 2 * ALLEGRO_PI * i / bulletCount;
        OrbitBullet* ob = new OrbitBullet(angle, 100 + 20*level, 4, player, this);
        getPlayScene()->BulletGroup->AddNewObject(ob);
        oldbullet.push_back(ob);
    }
    // AudioHelper::PlayAudio("gun.wav");
}

void CircleWeapon::Update(float Deltatime) {
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();

    if (!created && game.my_id == owner_id) {
        this->CreateBullet();
        created = true;
    } 
}

void CircleWeapon::Draw() const {
    //don't draw
}
void CircleWeapon::Upgrade() {
    Weapon::Upgrade();
    
    for (auto ob: oldbullet) {
        getPlayScene()->BulletGroup->RemoveObject(ob->GetObjectIterator());
    }
    oldbullet.clear();
    
    CreateBullet();
}