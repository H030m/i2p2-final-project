#include <algorithm>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <cmath>
#include <fstream>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include "Enemy/Enemy.hpp"
#include "Enemy/SoldierEnemy.hpp"
//TODO HACKATHON-3
#include "Enemy/PlaneEnemy.hpp"
#include "Enemy/TankEnemy.hpp"
#include "Enemy/BossEnemy.hpp"
#include "Enemy/ArmoredEnemy.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Turret/LaserTurret.hpp"
#include "Turret/MachineGunTurret.hpp"
#include "Turret/AbsorptionTurret.hpp"
#include "Turret/TurretButton.hpp"
#include "Turret/Shovel.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/Plane.hpp"
#include "UI/Component/Label.hpp"
#include "SubmitScene.hpp"
#include "WinScene.hpp"
#include "Map/Texture.hpp"
#include "Player/Player.hpp"
#include "Weapon/Weapon.hpp"
#include "Weapon/GunWeapon.hpp"
#include "Weapon/ShotgunWeapon.hpp"
#include "Weapon/CircleWeapon.hpp"
#include "Weapon/BounceWeapon.hpp"
#include "Camera/Camera.hpp"

static bool isNumber(const std::string& s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = { Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1) };
std::unordered_map<int, std::string> weapon_dict = {{1, "Gun Weapon"}, {2, "Shotgun Weapon"}, {3, "Orbit Weapon"}, {4, "Bounce Weapon"}};
const int PlayScene::BlockSize = 64;
const Engine::Point TileSize(65,65);
const float PlayScene::DangerTime = 7.61;
const Engine::Point PlayScene::SpawnGridPoint = Engine::Point(-1, 0);
int PlayScene::MapWidth = 20;
int PlayScene::MapHeight = 13;
// key sequence
const std::vector<int> PlayScene::code = {
    ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN,
    ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT,
    ALLEGRO_KEY_B, ALLEGRO_KEY_A, 216, ALLEGRO_KEY_ENTER
};

Engine::Point PlayScene::GetClientSize() {
    return Engine::Point(MapWidth * BlockSize,  MapHeight * BlockSize);
}

void PlayScene::Initialize() {
    mapState.clear();
    keyStrokes.clear();
    ticks = 0;
    deathCountDown = -1;
    lives = 10;
    money = 150;
    SpeedMult = 1;
    time(&StartTime); 
    // Add groups from bottom to top.
    AddNewObject(TileMapGroup = new Group());
    AddNewObject(ObstacleGroup = new Group());
    AddNewObject(GroundEffectGroup = new Group());
    AddNewObject(DebugIndicatorGroup = new Group());
    // AddNewObject(TowerGroup = new Group());
    AddNewObject(EnemyGroup = new Group());
    AddNewObject(BulletGroup = new Group());
    AddNewObject(EffectGroup = new Group());
    AddNewObject(PlayerGroup = new Group());
    AddNewObject(WeaponGroup = new Group());
    AddNewControlObject(UIGroup = new Group());

    ReadMap();
    

    {
        Engine::GameEngine &game = Engine::GameEngine::GetInstance();
        Player* newPlayer = new Player(500, 500, game.my_id,MapWidth, MapHeight);
        my_id = game.my_id;
        PlayerGroup->AddNewObject(newPlayer);
        player_dict[game.my_id] = newPlayer;
        
        //some UI
        Engine::Label* label;
        player_UI_Label[game.my_id].push_back(label = new Engine::Label("YOUUU", "pirulen.ttf", 48, 0, 0, 255, 255, 255, 255, 0, 0));
        label->fixed = true;
        UIGroup->AddNewObject(label);
        player_UI_Label[game.my_id].push_back(label = new Engine::Label("hello", "pirulen.ttf", 24, 10, 50, 255, 255, 255, 255, 0, 0));
        label->fixed = true;
        UIGroup->AddNewObject(label);

         // upgrade button
        Engine::ImageButton* btn;
        player_UI_Button[game.my_id].push_back(btn = new Engine::ImageButton("play/sand.png", "play/floor.png", 20, game.screenH - 140, 250, 30));
        btn->fixed = true;
        btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 1));
        UIGroup->AddNewControlObject(btn);
        player_UI_Button[game.my_id].push_back(btn = new Engine::ImageButton("play/sand.png", "play/floor.png", 20, game.screenH - 70, 250, 30));
        btn->fixed = true;
        btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 2));
        UIGroup->AddNewControlObject(btn);

        // camera
        
        camera = std::make_unique<Camera>(game.screenW, game.screenH, Engine::Point(MapWidth*BlockSize,MapHeight*BlockSize));
        camera->SetTarget(newPlayer->Position);

        //Weapon
        Weapon* k = new BounceWeapon(0, 0, my_id, 1);
        WeaponGroup->AddNewObject(k);
        newPlayer->Weapon_hold.push_back(k);
        newPlayer->Weapon_owned.push_back(k);
        k = new ShotgunWeapon(0, 0, my_id, 1);
        WeaponGroup->AddNewObject(k);
        newPlayer->Weapon_hold.push_back(k);
        newPlayer->Weapon_owned.push_back(k);

        upgrade_label_1 = new Engine::Label("", "pirulen.ttf", 15, 30, game.screenH - 130, 0, 0, 0, 255, 0, 0);
        upgrade_label_1->Text = weapon_dict[newPlayer->Weapon_hold[0]->type] + " (" + std::to_string(newPlayer->Weapon_hold[0]->level) +")";
        upgrade_label_1->fixed = true; 
        upgrade_label_2 = new Engine::Label("", "pirulen.ttf", 15, 30, game.screenH - 60, 0, 0, 0, 255, 0, 0);
        upgrade_label_2->Text = weapon_dict[newPlayer->Weapon_hold[1]->type] + " (" + std::to_string(newPlayer->Weapon_hold[1]->level) +")";
        upgrade_label_2->fixed = true;
        UIGroup->AddNewObject(upgrade_label_1); UIGroup->AddNewObject(upgrade_label_2);
    }

    
    
    std::cerr<<"hi\n";
    ReadEnemyWave();
    mapDistance = CalculateBFSDistance();
    ConstructUI();
    imgTarget = new Engine::Image("play/target.png", 0, 0);
    imgTarget->Visible = false;
    // preview = nullptr;
    UIGroup->AddNewObject(imgTarget);
    // Preload Lose Scene
    deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
    Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
    // Start BGM.
    bgmId = AudioHelper::PlayBGM("play.ogg");

    // DEBUG add one enemy
    // ArmoredEnemy* e = new ArmoredEnemy(200, 200);
    // e->UpdatePath(mapDistance);
    // EnemyGroup->AddNewObject(e);
}
void PlayScene::Terminate() {
    AudioHelper::StopBGM(bgmId);
    AudioHelper::StopSample(deathBGMInstance);
    deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

void PlayScene::Update(float deltaTime) {
  
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    GameClient &sender = game.GetSender();
    
    // record active player
    std::set<int> activePlayerIds;
    activePlayerIds.insert(game.my_id);
    
    // itrate through all players
    for (auto [_id, client_info] : sender.input_json.items()) {
        bool isNewPlayer = false;
        if (!isNumber(_id)) continue;
        int id = std::stoi(_id);
        
        // player marked active
        activePlayerIds.insert(id);

        if (!client_info.contains("player") || !client_info["player"].is_array() || client_info["player"].size() < 4 || client_info["player"][2] != state)
            continue;

        float x = client_info["player"][0];
        float y = client_info["player"][1];

        auto it = player_dict.find(id);
        
        if (it == player_dict.end()) {
            isNewPlayer = true;
            Player* newPlayer = new Player(x, y, id);
            PlayerGroup->AddNewObject(newPlayer);
            player_dict[id] = newPlayer;
            newPlayer->status = client_info["player"][3];
            newPlayer->health = client_info["player"][4];

            player_UI_Label[id].push_back(new Engine::Label("", "pirulen.ttf", 48, 0, 50, 0, 0, 0, 255, 0, 0));
            player_UI_Label[id].push_back(new Engine::Label("", "pirulen.ttf", 24, 10, 50, 0, 0, 0, 255, 0, 0));
            player_UI_Label[id][0]->fixed = true; player_UI_Label[id][1]->fixed = true;
            UIGroup->AddNewObject(player_UI_Label[id][0]);
            UIGroup->AddNewObject(player_UI_Label[id][1]);
        } else {
            if(id == game.my_id) continue;
            it->second->Position.x = x;
            it->second->Position.y = y;
            it->second->status = client_info["player"][3];
            it->second->health = client_info["player"][4];
        }
        //weapon
        if(id != game.my_id){
            if (client_info.contains("weapon") && client_info["weapon"].is_array()) {
                std::vector<int> weapons;
                for (auto& weapon : client_info["weapon"]) {
                    if (weapon.is_number_integer()) {
                        weapons.push_back(weapon.get<int>());
                    }
                }

                if (!isNewPlayer) {
                    for (Weapon* w : player_dict[id]->Weapon_hold) {
                        WeaponGroup->RemoveObject(w->GetObjectIterator());
                    
                    }   
                    player_dict[id]->Weapon_hold.clear();
                }
                
                Weapon* ww;
                for (auto weapontype : weapons) {
                    switch(weapontype) {
                       case(1) :
                            WeaponGroup->AddNewObject(ww = new GunWeapon(0, 0, id, 1)); player_dict[id]->Weapon_hold.push_back(ww);
                            break;
                       case(2) :
                            WeaponGroup->AddNewObject(ww = new ShotgunWeapon(0, 0, id, 1)); player_dict[id]->Weapon_hold.push_back(ww);
                            break;
                        case(3) :
                            WeaponGroup->AddNewObject(ww = new CircleWeapon(0, 0, id, 1)); player_dict[id]->Weapon_hold.push_back(ww);
                            break;
                        case(4) :
                            WeaponGroup->AddNewObject(ww = new BounceWeapon(0, 0, id, 1)); player_dict[id]->Weapon_hold.push_back(ww);
                            break;
                    }
                }
            }
        }
        
    }
    
    // delete not active player
    for (auto it = player_dict.begin(); it != player_dict.end(); ) {
        int playerId = it->first;

        if (playerId == game.my_id) {
            ++it;
            continue;
        }

        if (activePlayerIds.find(playerId) == activePlayerIds.end()) {
            std::cout << "[Remove] removing player id: " << playerId << std::endl;

            auto objs = PlayerGroup->GetObjects();
            for(auto obj: objs) {
                if(obj == it->second){
                    obj->GetObjectIterator()->first = false;
                    PlayerGroup->RemoveObject(obj->GetObjectIterator());
                }
            }

            if (WeaponGroup) {
                for (Weapon* w : it->second->Weapon_hold) {
                    WeaponGroup->RemoveObject(w->GetObjectIterator());
                }
            }
            it->second->Weapon_hold.clear();
            delete it->second;

            if (player_UI_Label.count(playerId)) {
                for (auto* label : player_UI_Label[playerId]) {
                    if (UIGroup) {
                        UIGroup->RemoveObject(label->GetObjectIterator());
                    }
                }
                player_UI_Label.erase(playerId);
            }

            it = player_dict.erase(it);
        } else {
            ++it;
        }
    }
    
    // update myself
    if (player_dict.find(game.my_id) != player_dict.end()) {
        player_dict[game.my_id]->UpdateMyPlayer(deltaTime);
        sender.output_json["player"] = {player_dict[game.my_id]->Position.x, player_dict[game.my_id]->Position.y, state,
                                        player_dict[game.my_id]->status, player_dict[game.my_id]->health};
                                    
        for (auto n: player_dict[game.my_id]->Weapon_hold) {
            sender.output_json["weapon"].push_back(n->type);
        }
    
    }

    // update UI
    int count = 1;
    for (auto &labels: player_UI_Label) {
        int curid = labels.first;
        Engine::Label* name_label = labels.second[0];
        Engine::Label* health_label = labels.second[1];

        if (curid == my_id) {
            name_label->Position.y = 0;
            health_label->Position.y = 50;
            health_label->Text = "Health : " + std::to_string(player_dict[curid]->health);
            continue;
        }
        
        name_label->Position.y = 96*count;
        name_label->Text = "player_" + std::to_string(curid);
        health_label->Position.y = 96*count + 50;
        health_label->Text = "Health : " + std::to_string(player_dict[curid]->health);
        count++;
    }

    // enemy
    std::unordered_set<int>NotFoundEnemy;
    for(auto it: enemy_dict)NotFoundEnemy.insert(it.first);
    if(sender.input_json.contains("-1")) { // -1 is enemy
        auto enemies = sender.input_json["-1"];
        
        for(auto enemy : enemies) {
            if(NotFoundEnemy.count(enemy["id"])){
                NotFoundEnemy.erase(enemy["id"]);
            }
            if(enemy_dict.count(enemy["id"])) {
                // enemy has existed
                enemy_dict[enemy["id"]]->Position.x = enemy["x"];
                enemy_dict[enemy["id"]]->Position.y = enemy["y"];
                enemy_dict[enemy["id"]]->Rotation = enemy["rotation"];
                enemy_dict[enemy["id"]]->hp = (enemy["alive"] == true)? 1 : 0;
                enemy_dict[enemy["id"]]->id = enemy["id"];
                enemy_dict[enemy["id"]]->damage = enemy["damage"];

            } else{
                //create new enemy
                Enemy* newEnemy;
                switch ((int)enemy["enemyType"])
                {
                case 0:
                    newEnemy = new ArmoredEnemy(enemy["x"],enemy["y"]);
                    EnemyGroup->AddNewObject(newEnemy);
                    enemy_dict[enemy["id"]] = newEnemy;
                    break;
                default:
                    break;
                }
            }

        }
    }
    for(auto it:NotFoundEnemy){
        EnemyGroup->RemoveObject(enemy_dict[it]);
        enemy_dict.erase(it);
    }

    // scene
    camera->SetTarget(player_dict[my_id]->Position);
    camera->Update(deltaTime);
    IScene::Update(deltaTime);


}
void PlayScene::Draw() const {
    // IScene::Draw();
    RenderVisibleTiles();
    RenderVisibleObjects();

    UIGroup->Draw();
    if (DebugMode) {
        // Draw reverse BFS distance on all reachable blocks.
        for (int i = 0; i < MapHeight; i++) {
            for (int j = 0; j < MapWidth; j++) {
                if (mapDistance[i][j] != -1) {
                    // Not elegant nor efficient, but it's quite enough for debugging.
                    Engine::Label label(std::to_string(mapDistance[i][j]), "pirulen.ttf", 32, (j + 0.5) * BlockSize, (i + 0.5) * BlockSize);
                    label.Anchor = Engine::Point(0.5, 0.5);
                    label.Draw();
                }
            }
        }
    }
}

void PlayScene::RenderVisibleTiles() const {
    auto visibleArea = camera->GetVisibleTileArea(BlockSize);
    
    // �u��V�i�����ˤ�
    for (auto obj : TileMapGroup->GetObjects()) {
        Engine::Sprite* sprite = dynamic_cast<Engine::Sprite*>(obj);
        if (!sprite) continue;
        
        // �ˬd�ˤ��O�_�b�i���d��
        if (camera->IsInView(sprite->Position, BlockSize)) {
            // �N�@�ɮy���ഫ���ù��y��
            Engine::Point screenPos = camera->WorldToScreen(sprite->Position);
            
            // �Ȯɭק���F��m�i���V
            Engine::Point originalPos = sprite->Position;
            sprite->Position = screenPos;
            sprite->Draw();
            sprite->Position = originalPos;
        }
    }
}

void PlayScene::RenderVisibleObjects() const {
   
    std::vector<Group*> renderGroups = {
        GroundEffectGroup, DebugIndicatorGroup, ObstacleGroup, EnemyGroup, 
        BulletGroup, EffectGroup, PlayerGroup, WeaponGroup
    };
    
    for (Group* group : renderGroups) {
        for (auto obj : group->GetObjects()) {
            if (camera->IsInView(obj->Position, 64)) {
                Engine::Point screenPos = camera->WorldToScreen(obj->Position);
                Engine::Point originalPos = obj->Position;
                obj->Position = screenPos;
                obj->Draw();
                obj->Position = originalPos;
            }
        }
    }
}

void PlayScene::OnMouseDown(int button, int mx, int my) {
    IScene::OnMouseDown(button, mx, my);
}
void PlayScene::OnMouseMove(int mx, int my) {
    IScene::OnMouseMove(mx, my);
}
void PlayScene::OnMouseUp(int button, int mx, int my) {
    IScene::OnMouseUp(button, mx, my);
    
}
void PlayScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    for (auto n: PlayerGroup->GetObjects()) {
        Player* player = dynamic_cast<Player*>(n);
        player->OnKeyDown(keyCode);
    }
}
void PlayScene::OnKeyUp(int keyCode) {
    for (auto n: PlayerGroup->GetObjects()) {
        Player* player = dynamic_cast<Player*>(n);
        player->OnKeyUp(keyCode);
    }
    
}
void PlayScene::Hit() {
    lives--;
    UILives->Text = std::string("Life ") + std::to_string(lives);
    if (lives <= 0) {
        Engine::GameEngine::GetInstance().ChangeScene("lose");
    }
}
int PlayScene::GetMoney() const {
    return money;
}
void PlayScene::EarnMoney(int money) {
    this->money += money;
    UIMoney->Text = std::string("$") + std::to_string(this->money);
}
void PlayScene::ReadMap() {

    std::cerr<<"hellow!\n";
    std::string filename = "loadingMap.json"; // or ".json"
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open map file: " << filename << '\n';
        return;
    }

    nlohmann::json data;
    file >> data;
    file.close();
    if (!data.contains("MapState")) {
        std::cerr << "Invalid map format: missing MapState\n";
        return;
    }
    if (!data.contains("MapWidth") || !data.contains("MapHeight")){
        std::cerr << "Invalid W and H\n";
    }
    MapHeight = data["MapHeight"];
    MapWidth = data["MapWidth"];

    auto& map = data["MapState"];
    mapState.resize(MapHeight, std::vector<nlohmann::json>(MapWidth));
    
    //Obstacle
    for (int i = 0; i < MapHeight; ++i) {
    for (int j = 0; j < MapWidth; ++j) {
        auto tile = map[i][j]["Tile"];
        mapState[i][j] = map[i][j];
        std::string file = tile["file_name"];
        int sx = tile["x"];
        int sy = tile["y"];
        int sw = tile["w"];
        int sh = tile["h"];

        auto* spr = new Engine::Sprite(
            file,
            j * BlockSize, i * BlockSize,
            0, 0,
            0, 0
        );
        spr->Size = TileSize;
        spr->SourceX = sw * sx + 1;
        spr->SourceY = sh * sy + 1;
        spr->SourceW = sw - 2;
        spr->SourceH = sh - 2;
        TileMapGroup->AddNewObject(spr);

        // 
        if (map[i][j].contains("Obstacle")) {
            auto& obs = map[i][j]["Obstacle"];
            if (obs.contains("file_name")) {
                std::string obs_file = obs["file_name"];
                float ox = obs["x"];
                float oy = obs["y"];
                float ow = obs["w"];
                float oh = obs["h"];

                auto* obs_spr = new Engine::Sprite(
                    obs_file,
                    j * BlockSize, i * BlockSize,
                    0, 0,
                    0, 0
                );
                obs_spr->Size = TileSize;
                obs_spr->SourceX = ow * ox + 1;
                obs_spr->SourceY = oh * oy + 1;
                obs_spr->SourceW = ow - 2;
                obs_spr->SourceH = oh - 2;
                if(obs.contains("SizeX") && obs.contains("SizeY")){
                    obs_spr->Size.x = obs["SizeX"];
                    obs_spr->Size.y = obs["SizeY"];
                }
                if(obs["Obstacle"].contains("OffsetX")) obs_spr->Position.x += (int)obs["Obstacle"]["OffsetX"];
                if(obs["Obstacle"].contains("OffsetY")) obs_spr->Position.x += (int)obs["Obstacle"]["OffsetY"];
                ObstacleGroup->AddNewObject(obs_spr);
            }
        }
    }
}
}
void PlayScene::ReadEnemyWave() {
    std::string filename = std::string("Resource/enemy") + std::to_string(MapId) + ".txt";
    // Read enemy file.
    float type, wait, repeat;
    enemyWaveData.clear();
    std::ifstream fin(filename);
    while (fin >> type && fin >> wait && fin >> repeat) {
        for (int i = 0; i < repeat; i++)
            enemyWaveData.emplace_back(type, wait);
    }
    fin.close();
}
void PlayScene::ConstructUI() {
   
}

void PlayScene::UIBtnClicked(int id) {
   if (id == 1) {
        player_dict[my_id]->Weapon_hold[0]->Upgrade();
        upgrade_label_1->Text = weapon_dict[player_dict[my_id]->Weapon_hold[0]->type] + " (" + std::to_string(player_dict[my_id]->Weapon_hold[0]->level) +")";
    }
    else if (id == 2) {
        player_dict[my_id]->Weapon_hold[1]->Upgrade();
        upgrade_label_2->Text = weapon_dict[player_dict[my_id]->Weapon_hold[1]->type] + " (" + std::to_string(player_dict[my_id]->Weapon_hold[1]->level) +")";
    }
}

bool PlayScene::CheckSpaceValid(int x, int y) {
    return true;
}
std::vector<std::vector<int>> PlayScene::CalculateBFSDistance() {
    // // Reverse BFS to find path.
    std::vector<std::vector<int>> map(MapHeight, std::vector<int>(std::vector<int>(MapWidth, -1)));
    return map;
}

bool PlayScene::isWalkable(int x, int y, int radius) {
    // Convert pixel-based position and radius into tile coordinate range
    int left = std::max(0, (x - radius) / BlockSize);
    int right = std::min(MapWidth - 1, (x + radius) / BlockSize);
    int top = std::max(0, (y - radius) / BlockSize);
    int bottom = std::min(MapHeight - 1, (y + radius) / BlockSize);

    // Iterate through the square area of tiles that might intersect with the circular area
    for (int j = top; j <= bottom; ++j) {
        for (int i = left; i <= right; ++i) {
            // Calculate the center of the current tile
            int tileCenterX = i * BlockSize + BlockSize / 2;
            int tileCenterY = j * BlockSize + BlockSize / 2;

            // Check if the tile is within the circle using distance squared
            int dx = tileCenterX - x;
            int dy = tileCenterY - y;
            if (dx * dx + dy * dy <= radius * radius) {
                // Check if the tile contains a non-penetrable obstacle
                if (mapState[j][i].contains("Obstacle") &&
                    mapState[j][i]["Obstacle"].contains("Penetrable") &&
                    mapState[j][i]["Obstacle"]["Penetrable"] == false) {
                    return false; // Found a blocking tile
                }
            }
        }
    }

    return true; // No blocking tiles found
}