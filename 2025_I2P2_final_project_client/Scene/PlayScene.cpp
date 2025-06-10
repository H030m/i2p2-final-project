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
bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = { Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1) };
const int PlayScene::MapWidth = 20, PlayScene::MapHeight = 13;
const int PlayScene::BlockSize = 64;
const float PlayScene::DangerTime = 7.61;
const Engine::Point PlayScene::SpawnGridPoint = Engine::Point(-1, 0);
const Engine::Point PlayScene::EndGridPoint = Engine::Point(MapWidth, MapHeight - 1);

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
    AddNewObject(TowerGroup = new Group());
    AddNewObject(EnemyGroup = new Group());
    AddNewObject(BulletGroup = new Group());
    AddNewObject(EffectGroup = new Group());
    AddNewObject(PlayerGroup = new Group());
    {
        Engine::GameEngine &game = Engine::GameEngine::GetInstance();
        Player* newPlayer = new Player(100, 100, game.my_id);
        PlayerGroup->AddNewObject(newPlayer);
        player_dict[game.my_id] = newPlayer;
        
    }
    // Should support buttons.
    AddNewControlObject(UIGroup = new Group());
    ReadMap();
    std::cerr<<"hi\n";
    ReadEnemyWave();
    mapDistance = CalculateBFSDistance();
    ConstructUI();
    imgTarget = new Engine::Image("play/target.png", 0, 0);
    imgTarget->Visible = false;
    preview = nullptr;
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

        if (!client_info.contains("player") || !client_info["player"].is_array() || client_info["player"].size() < 2)
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
    }
    
    IScene::Update(deltaTime);
}
void PlayScene::Draw() const {
    IScene::Draw();
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
    std::string filename = std::string("Resource/map") + std::to_string(MapId) + ".json";
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file.\n";
        return;
    }

    json data;
    file >> data;

    mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth,TILE_DIRT));
    for (int i = 0; i < MapHeight; ++i) {
        for (int j = 0; j < MapWidth; ++j) {
            int linearIndex = i + j * MapHeight;
            std::string key = std::to_string(linearIndex);

            if (!data.contains(key)) {
                std::cerr << "Missing tile at index " << key << "\n";
                mapState[i][j] = TILE_DIRT;
                continue;
            }

            int texValue = data[key]["texture"];
            mapState[i][j] = texValue ? TILE_FLOOR : TILE_DIRT;
        }
    }
    std::vector<std::vector<int>>mapState_2(MapHeight, std::vector<int>(MapWidth));
    for (int i = 0; i < MapHeight; ++i) {
        for (int j = 0; j < MapWidth; ++j) {
            mapState_2[i][j] = mapState[i][j] == TILE_FLOOR ? 1 : 0;
        }
    }
    for (int i = 0; i < MapHeight; ++i) {
        for (int j = 0; j < MapWidth; ++j) {
            int linearIndex = i + j * MapHeight;
            std::string key = std::to_string(linearIndex);

            if (!data.contains(key)) {
                std::cerr << "Missing tile at index " << key << "\n";
                continue;
            }

            int texValue = data[key]["texture"];
            mapState[i][j] = texValue ? TILE_FLOOR : TILE_DIRT;

            // Get (tileX, tileY) in tile sheet
            auto [tileX, tileY] = getTileCoord(i,j,mapState_2, MapHeight, MapWidth);
            
            const int TileWidth = 16;
            const int TileHeight = 16;

            // Create sprite and set source region
            auto* spr = new Engine::Sprite(
                "play/grass/" + std::to_string(tileY) + std::to_string(tileX) + ".png",
                j * BlockSize, i * BlockSize,       // screen position
                BlockSize,BlockSize,               // draw size
                0, 0                                 // anchor top-left
            );

            TileMapGroup->AddNewObject(spr);
        }
        std::cerr<<'\n';
    }
        // std::cerr<<"h1\n";
        // {
        //     auto [tileX, tileY] = std::make_pair(0,0);
        //     const int TileWidth = 16;
        //     const int TileHeight = 16;

        //     // Create sprite and set source region
        //     auto* spr = new Engine::Sprite(
        //         "play/grass.png",
        //         1 * BlockSize, 1 * BlockSize,       // screen position
        //         0, 0,               // draw size
        //         0, 0                                 // anchor top-left
        //     );
        //  std::cerr<<"h3\n";
        //     // spr->SourceX = 10;
        //     // spr->SourceY = 0;
        //     // spr->SourceW = 16;
        //     // spr->SourceH = 16;

        //     TileMapGroup->AddNewObject(spr);
        //  std::cerr<<"h5\n";
        // }
        //  {
        //     auto [tileX, tileY] = std::make_pair(2,0);
        //     const int TileWidth = 16;
        //     const int TileHeight = 16;

        //     // Create sprite and set source region
        //     auto* spr = new Engine::Sprite(
        //         "play/grass.png",
        //        3 * BlockSize, 1 * BlockSize,       // screen position
        //         BlockSize, BlockSize,               // draw size
        //         0, 0                                 // anchor top-left
        //     );
        //  std::cerr<<"h3\n";
        //     spr->SourceX = 16;
        //     spr->SourceY = 0;
        //     spr->SourceW = TileWidth;
        //     spr->SourceH = TileHeight;

        //     TileMapGroup->AddNewObject(spr);
        //  std::cerr<<"h5\n";
        // }
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
    // if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
    //     return false;
    // auto map00 = mapState[y][x];
    // mapState[y][x] = TILE_OCCUPIED;
    // std::vector<std::vector<int>> map = CalculateBFSDistance();
    // mapState[y][x] = map00;
    // if (map[0][0] == -1)
    //     return false;
    // for (auto &it : EnemyGroup->GetObjects()) {
    //     Engine::Point pnt;
    //     pnt.x = floor(it->Position.x / BlockSize);
    //     pnt.y = floor(it->Position.y / BlockSize);
    //     if (pnt.x < 0) pnt.x = 0;
    //     if (pnt.x >= MapWidth) pnt.x = MapWidth - 1;
    //     if (pnt.y < 0) pnt.y = 0;
    //     if (pnt.y >= MapHeight) pnt.y = MapHeight - 1;
    //     if (map[pnt.y][pnt.x] == -1)
    //         return false;
    // }
    // // All enemy have path to exit.
    // mapState[y][x] = TILE_OCCUPIED;
    // mapDistance = map;
    // for (auto &it : EnemyGroup->GetObjects())
    //     dynamic_cast<Enemy *>(it)->UpdatePath(mapDistance);
    return true;
}
std::vector<std::vector<int>> PlayScene::CalculateBFSDistance() {
    // // Reverse BFS to find path.
    std::vector<std::vector<int>> map(MapHeight, std::vector<int>(std::vector<int>(MapWidth, -1)));
    // std::queue<Engine::Point> que;
    // // Push end point.
    // // BFS from end point.
    // if (mapState[MapHeight - 1][MapWidth - 1] != TILE_DIRT)
    //     return map;
    // que.push(Engine::Point(MapWidth - 1, MapHeight - 1));
    // map[MapHeight - 1][MapWidth - 1] = 0;
    // while (!que.empty()) {
    //     Engine::Point p = que.front();
    //     que.pop();
    //     // TODO PROJECT-1 (1/1): Implement a BFS starting from the most right-bottom block in the map.
    //     //               For each step you should assign the corresponding distance to the most right-bottom block.
    //     //               mapState[y][x] is TILE_DIRT if it is empty.
    //     int x[4] = {0,0,1,-1}, y[4] = {1,-1,0,0};
    //     for(int i = 0; i < 4; i++){
    //         int nx = x[i] + p.x, ny = y[i] + p.y;
    //         if (nx < 0 || nx >= MapWidth || ny < 0 || ny >= MapHeight)
    //             continue;
    //         if(mapState[ny][nx] == TILE_DIRT && map[ny][nx] == -1){
    //             map[ny][nx] = map[p.y][p.x] + 1;
    //             que.push(Engine::Point(nx, ny));
    //         }
    //     }
    // }
    return map;
}
