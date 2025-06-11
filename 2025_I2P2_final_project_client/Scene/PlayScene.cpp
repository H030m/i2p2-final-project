#include <algorithm>
#include <allegro5/allegro.h>
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
    return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
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
    AddNewObject(GroundEffectGroup = new Group());
    AddNewObject(DebugIndicatorGroup = new Group());
    // AddNewObject(TowerGroup = new Group());
    AddNewObject(EnemyGroup = new Group());
    AddNewObject(BulletGroup = new Group());
    AddNewObject(EffectGroup = new Group());
    AddNewObject(PlayerGroup = new Group());
    AddNewObject(WeaponGroup = new Group());

    ReadMap();

    WeaponGroup->AddNewObject(new ShotgunWeapon(100, 100));
    WeaponGroup->AddNewObject(new CircleWeapon(100, 100));
    WeaponGroup->AddNewObject(new GunWeapon(100, 100));
    WeaponGroup->AddNewObject(new BounceWeapon(100, 100));
    {
        Engine::GameEngine &game = Engine::GameEngine::GetInstance();
        Player* newPlayer = new Player(500, 500, game.my_id);
        my_id = game.my_id;
        PlayerGroup->AddNewObject(newPlayer);
        player_dict[game.my_id] = newPlayer;
        
        // cameraAdd commentMore actions
        
        camera = std::make_unique<Camera>(game.screenW, game.screenH, Engine::Point(MapWidth*BlockSize,MapHeight*BlockSize));
        camera->SetTarget(newPlayer->Position);
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
        if (_id == "my_id") continue;
        int id = std::stoi(_id);
        
        // player marked active
        activePlayerIds.insert(id);

        if (!client_info.contains("player") || !client_info["player"].is_array() || client_info["player"].size() < 3 || client_info["player"][2] != state)
            continue;

        float x = client_info["player"][0];
        float y = client_info["player"][1];

        auto it = player_dict.find(id);
        if (it == player_dict.end()) {
            Player* newPlayer = new Player(x, y, id);
            PlayerGroup->AddNewObject(newPlayer);
            player_dict[id] = newPlayer;
        } else {
            if(id == game.my_id) continue;
            it->second->Position.x = x;
            it->second->Position.y = y;
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
        sender.output_json["player"] = {player_dict[game.my_id]->Position.x, player_dict[game.my_id]->Position.y, state};
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
    std::string filename = std::string("Resource/map") + std::to_string(MapId) + ".json"; // or ".json"
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open map file: " << filename << '\n';
        return;
    }

    nlohmann::json data;
    file >> data;
    file.close();
    std::cerr<<data.dump()<<'\n';
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
                0,0,
                0, 0
            );
            spr->Size = TileSize;
            spr->SourceX = sw * sx + 1;
            spr->SourceY = sh * sy + 1;
            spr->SourceW = sw - 2;
            spr->SourceH = sh - 2;
            TileMapGroup->AddNewObject(spr);
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

