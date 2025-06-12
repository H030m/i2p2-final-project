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
    

    ReadMap();
    // EnemyGroup->AddNewObject(new ArmoredEnemy("play/enemy-1.png", 200, 200, 10, 5, 50, 100, 50));
    // WeaponGroup->AddNewObject(new ShotgunWeapon(100, 100));
    // WeaponGroup->AddNewObject(new CircleWeapon(100, 100));
    // WeaponGroup->AddNewObject(new GunWeapon(100, 100));
    // WeaponGroup->AddNewObject(new BounceWeapon(100, 100));

    {
        Engine::GameEngine &game = Engine::GameEngine::GetInstance();
        Player* newPlayer = new Player(500, 500, game.my_id,MapWidth, MapHeight);
        my_id = game.my_id;
        PlayerGroup->AddNewObject(newPlayer);
        player_dict[game.my_id] = newPlayer;
        
        // camera
        
        camera = std::make_unique<Camera>(game.screenW, game.screenH, Engine::Point(MapWidth*BlockSize,MapHeight*BlockSize));
        camera->SetTarget(newPlayer->Position);

         //Weapon
        Weapon* k = new ShotgunWeapon(0, 0, my_id);
        WeaponGroup->AddNewObject(k);
        newPlayer->Weapon_hold.push_back(k);
        newPlayer->Weapon_owned.push_back(k);
        k = new CircleWeapon(0, 0, my_id);
        WeaponGroup->AddNewObject(k);
        newPlayer->Weapon_hold.push_back(k);
        newPlayer->Weapon_owned.push_back(k);
    }

    // Should support buttons.
    AddNewControlObject(UIGroup = new Group());
    
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

        if (!client_info.contains("player") || !client_info["player"].is_array() || client_info["player"].size() < 3 || client_info["player"][2] != state)
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
        } else {
            if(id == game.my_id) continue;
            it->second->Position.x = x;
            it->second->Position.y = y;
            it->second->status = client_info["player"][3];
        }
        if (client_info.contains("weapon") && client_info["weapon"].is_array()) {
            std::vector<int> weapons;
            for (auto& weapon : client_info["weapon"]) {
                if (weapon.is_number_integer()) {
                    weapons.push_back(weapon.get<int>());
                }
            }

            if (!isNewPlayer) {
                for (Weapon* w : player_dict[id]->Weapon_hold) {
                    WeaponGroup->RemoveObject(w);  // 從 group 移除
                    delete w;                      // 釋放記憶體
                }   
                player_dict[id]->Weapon_hold.clear();
            }
            
            Weapon* ww;
            for (auto weapontype : weapons) {
                switch(weapontype) {
                    case(1) : 
                        WeaponGroup->AddNewObject(ww = new GunWeapon(0, 0, id)); player_dict[id]->Weapon_hold.push_back(ww);
                        break;
                    case(2) : 
                        WeaponGroup->AddNewObject(ww = new ShotgunWeapon(0, 0, id)); player_dict[id]->Weapon_hold.push_back(ww);
                        break;
                    case(3) : 
                        WeaponGroup->AddNewObject(ww = new CircleWeapon(0, 0, id)); player_dict[id]->Weapon_hold.push_back(ww);
                        break;
                    case(4) : 
                        WeaponGroup->AddNewObject(ww = new BounceWeapon(0, 0, id)); player_dict[id]->Weapon_hold.push_back(ww);
                        break;
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
            it = player_dict.erase(it);
        } else {
            ++it;
        }
    }
    
    // update myself
    if (player_dict.find(game.my_id) != player_dict.end()) {
        player_dict[game.my_id]->UpdateMyPlayer(deltaTime);
        sender.output_json["player"] = {player_dict[game.my_id]->Position.x, player_dict[game.my_id]->Position.y, state, 
                                        player_dict[game.my_id]->status};
                                    
        for (auto n: player_dict[game.my_id]->Weapon_hold) {
            sender.output_json["weapon"].push_back(n->type);
        }
    }
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
    
    // 只渲染可見的瓦片
    for (auto obj : TileMapGroup->GetObjects()) {
        Engine::Sprite* sprite = dynamic_cast<Engine::Sprite*>(obj);
        if (!sprite) continue;
        
        // 檢查瓦片是否在可見範圍內
        if (camera->IsInView(sprite->Position, BlockSize)) {
            // 將世界座標轉換為螢幕座標
            Engine::Point screenPos = camera->WorldToScreen(sprite->Position);
            
            // 暫時修改精靈位置進行渲染
            Engine::Point originalPos = sprite->Position;
            sprite->Position = screenPos;
            sprite->Draw();
            sprite->Position = originalPos;
        }
    }
}

void PlayScene::RenderVisibleObjects() const {
    // 渲染可見的遊戲物件
    std::vector<Group*> renderGroups = {
        GroundEffectGroup, DebugIndicatorGroup, EnemyGroup, 
        BulletGroup, EffectGroup, PlayerGroup, WeaponGroup, ObstacleGroup
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
    std::string filename = std::string("Resource/map") + std::to_string(MapId) + ".json"; // or ".json"
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

        // === 新增障礙物繪製 ===
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