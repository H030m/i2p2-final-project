#define NOMINMAX
#include <algorithm>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <fstream>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "DrawMapScene.hpp"
#include "Player/Player.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Resources.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Map/Texture.hpp"
#include "Map/TextureButton.hpp"
#include "UI/Component/ImageButton.hpp"
#include "Camera/Camera.hpp"

int DrawMapScene::MapHeight,DrawMapScene::MapWidth;
const int DrawMapScene::BlockSize = 64;
const Engine::Point TileSize(65,65);
const int tileX = 7; 
const int tileY = 0; 
const int tileW = 32, tileH = 32;
const int Xgap = 8, Ygap = 2;
std::string filename = "play/grass/grounds.png";
std::string obstacle_filename = "play/grass/terrain.png";
static bool isNumber(const std::string& s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}
void DrawMapScene::Initialize() {
    save_cooldown = 0;
    SelectFile.resize(3);

    AddNewObject(TileMapGroup = new Group());
    AddNewControlObject(UIGroup = new Group());
    AddNewObject(PlayerGroup = new Group());
    AddNewObject(ObstacleGroup = new Group());
    AddNewControlObject(LabelGroup = new Group);
    AddNewObject(GroundEffectGroup = new Group);

    //Map
    
    std::string filename = "Resource/map" + std::to_string(MapNum) + ".json";
    std::ifstream fin(filename);
    nlohmann::json mapJson;
    if (fin.is_open()) {
        
        fin >> mapJson;
    } else {
        std::cerr << "Failed to open Resource/map2.json\n";
    }
    MapWidth = (int)mapJson["MapWidth"];
    MapHeight = (int)mapJson["MapHeight"];
    MapState.resize(MapHeight, std::vector<nlohmann::json>(MapWidth));
    for (int i = 0; i < MapHeight; ++i) {
        for (int j = 0; j < MapWidth; ++j) {
            MapState[i][j] = mapJson["MapState"][i][j];
        }
    }
    mapState_2.resize(MapHeight, std::vector<int>(MapWidth));
    ReadMap();


    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    {
        Player* newPlayer = new Player(500, 500, game.my_id, MapWidth, MapHeight);
        PlayerGroup->AddNewObject(newPlayer);
        player_dict[game.my_id] = newPlayer;
        //320 -> UI'bound
        camera = std::make_unique<Camera>(game.screenW - 320, game.screenH, Engine::Point(MapWidth*BlockSize,MapHeight*BlockSize));
        camera->SetTarget(newPlayer->Position);
    }

    //DrawUI
    preview = nullptr;
    imgTarget = new Engine::Image("play/target.png", 0, 0);
    imgTarget->Size = Engine::Point(64,64);
    imgTarget->Visible = false;
    imgTarget->fixed = true;
    UIGroup->AddNewObject(imgTarget);
    
    // correctpng
    correct = new Engine::Image("play/target.png",game.screenW/2,100,64,64,0.5f,0.5f);
    correct->Visible = false;
    UIGroup->AddNewObject(correct);
    std::cerr<<"hello correct\n";
    ConstructUI();

    
}

void DrawMapScene::Terminate() {
    IScene::Terminate();
}
void DrawMapScene::Update(float deltaTime) {
    save_cooldown -= deltaTime;
    if(save_cooldown < 0)save_cooldown = 0;
    correct_time -= deltaTime;
    if(correct_time <= 0 && correct != nullptr){
        correct->Visible = false;
        correct = 0;
    }
    // update client's W, A, S, D
    // PlayerGroup->Update(deltaTime);

    // // check new player join & update all players' position
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    GameClient &sender = game.GetSender();
    for (auto [_id, client_info] : sender.input_json.items()) {
        if (!isNumber(_id)) continue;
        int id = std::stoi(_id);
        if(id < 0)continue;//enemy
        if (!client_info.contains("player") || !client_info["player"].is_array() || client_info["player"].size() < 3 || client_info["player"][2] != -1)
            continue;

        float x = client_info["player"][0];
        float y = client_info["player"][1];

        auto it = player_dict.find(id);
        if (it == player_dict.end()) {
            Player* newPlayer = new Player(x, y);
            PlayerGroup->AddNewObject(newPlayer);
            player_dict[id] = newPlayer;
        } else {
            if(id == game.my_id)continue;
            it->second->Position.x = x;
            it->second->Position.y = y;
        }
        
    }
    // update myself
    if (player_dict.find(game.my_id) != player_dict.end()) {
        player_dict[game.my_id]->UpdateMyPlayer(deltaTime);
        sender.output_json["player"] = {player_dict[game.my_id]->Position.x, player_dict[game.my_id]->Position.y, -1};
    }
    // preview
    if (preview) {
        preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
        // To keep responding when paused.
        preview->Update(deltaTime);
    }
    camera->SetTarget(player_dict[game.my_id]->Position);
    camera->Update(deltaTime);
    IScene::Update(deltaTime);
}
void DrawMapScene::Draw() const {
    RenderVisibleTiles();
    RenderVisibleObjects();
}
void DrawMapScene::OnMouseDown(int button, int mx, int my) {
    if ((button & 1) && !imgTarget->Visible && preview) {
        // Cancel turret construct.
        UIGroup->RemoveObject(preview->GetObjectIterator());
        preview = nullptr;
    }
    IScene::OnMouseDown(button, mx, my);
}
void DrawMapScene::OnMouseMove(int mx, int my) {
    IScene::OnMouseMove(mx, my);
    const int x = mx / BlockSize;
    const int y = my / BlockSize;
   
    if (!preview || x < 0 || x >= 20 || y < 0 || y >= 13) {
        imgTarget->Visible = false;
        return;
    }
    imgTarget->Visible = true;
    imgTarget->Position.x = x * BlockSize;
    imgTarget->Position.y = y * BlockSize;
}

void DrawMapScene::OnMouseUp(int button, int mx, int my) {
    IScene::OnMouseUp(button, mx, my);
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    if (imgTarget->Visible) {
        Engine::Point worldPos = camera->ScreenToWorld(Engine::Point(mx, my));
int x = worldPos.x / BlockSize;
int y = worldPos.y / BlockSize;
        nlohmann::json tile = {
            {"Tile", {
                {"file_name", filename},
                {"x", 0},
                {"y", 0},
                {"w", tileW},
                {"h", tileH}
            }}
        };
        
        if(preview) {
            //grass
            if(preview->id == 0){
                MapState[y][x]["Tile"]["x"] = rand()%(Xgap-2) + 2 + Xgap*0;
                MapState[y][x]["Tile"]["y"] = rand()%(Ygap) + 0 + Ygap*0;
                MapState[y][x]["Tile"]["w"] = tileW;
                MapState[y][x]["Tile"]["h"] = tileH;
                TileMapGroup->RemoveObject(Tile_dict[y + x*MapWidth]->GetObjectIterator());
                auto* spr = new Engine::Sprite(
                    MapState[y][x]["Tile"]["file_name"],
                    x * BlockSize, y * BlockSize,       // screen position
                    0,0,               // draw size
                    0, 0                                 // anchor top-left
                );
                spr->Size = TileSize;
                spr->SourceH = (float)MapState[y][x]["Tile"]["h"] - 2;
                spr->SourceW = (float)MapState[y][x]["Tile"]["w"] - 2;
                spr->SourceX = (float)MapState[y][x]["Tile"]["w"]  * (float)MapState[y][x]["Tile"]["x"] + 1;
                spr->SourceY = (float)MapState[y][x]["Tile"]["h"]  * (float)MapState[y][x]["Tile"]["y"] + 1;
                TileMapGroup->AddNewObject(spr);
                Tile_dict[y + x*MapWidth] = spr;
            }
            //sand
            if(preview->id == 1){
                MapState[y][x]["Tile"]["x"] = rand()%(Xgap-2) + 2 + Xgap*1;
                MapState[y][x]["Tile"]["y"] = rand()%(Ygap) + 0 + Ygap*0;
                MapState[y][x]["Tile"]["w"] = tileW;
                MapState[y][x]["Tile"]["h"] = tileH;
                TileMapGroup->RemoveObject(Tile_dict[y + x*MapWidth]->GetObjectIterator());
                auto* spr = new Engine::Sprite(
                    MapState[y][x]["Tile"]["file_name"],
                    x * BlockSize, y * BlockSize,       // screen position
                    0,0,               // draw size
                    0, 0                                 // anchor top-left
                );
                spr->Size = TileSize;
                spr->SourceH = (float)MapState[y][x]["Tile"]["h"] - 2;
                spr->SourceW = (float)MapState[y][x]["Tile"]["w"] - 2;
                spr->SourceX = (float)MapState[y][x]["Tile"]["w"]  * (float)MapState[y][x]["Tile"]["x"] + 1;
                spr->SourceY = (float)MapState[y][x]["Tile"]["h"]  * (float)MapState[y][x]["Tile"]["y"] + 1;
                TileMapGroup->AddNewObject(spr);
                Tile_dict[y + x*MapWidth] = spr;
            }
            //stone
            if(preview->id == 2){
                MapState[y][x]["Tile"]["x"] = rand()%(Xgap-2) + 2 + Xgap*1;
                MapState[y][x]["Tile"]["y"] = rand()%(Ygap) + 0 + Ygap*1;
                MapState[y][x]["Tile"]["w"] = tileW;
                MapState[y][x]["Tile"]["h"] = tileH;
                TileMapGroup->RemoveObject(Tile_dict[y + x*MapWidth]->GetObjectIterator());
                auto* spr = new Engine::Sprite(
                    MapState[y][x]["Tile"]["file_name"],
                    x * BlockSize, y * BlockSize,       // screen position
                    0,0,               // draw size
                    0, 0                                 // anchor top-left
                );
                spr->Size = TileSize;
                spr->SourceH = (float)MapState[y][x]["Tile"]["h"] - 2;
                spr->SourceW = (float)MapState[y][x]["Tile"]["w"] - 2;
                spr->SourceX = (float)MapState[y][x]["Tile"]["w"]  * (float)MapState[y][x]["Tile"]["x"] + 1;
                spr->SourceY = (float)MapState[y][x]["Tile"]["h"]  * (float)MapState[y][x]["Tile"]["y"] + 1;
                TileMapGroup->AddNewObject(spr);
                Tile_dict[y + x*MapWidth] = spr;
            }
            // stone obstacle
            if(preview->id == 3){
                
                MapState[y][x]["Obstacle"]["x"] = rand()%(3) + 0 + Xgap*1;
                MapState[y][x]["Obstacle"]["y"] = rand()%(3) * Ygap;
                MapState[y][x]["Obstacle"]["w"] = tile["Tile"]["w"];
                MapState[y][x]["Obstacle"]["h"] = tile["Tile"]["h"];
                MapState[y][x]["Obstacle"]["file_name"] = obstacle_filename;
                MapState[y][x]["Obstacle"]["Penetrable"] = false;
                if(Obstacle_dict.count(y + x*MapWidth))
                ObstacleGroup->RemoveObject(Obstacle_dict[y + x*MapWidth]->GetObjectIterator());
                auto* spr = new Engine::Sprite(
                    MapState[y][x]["Obstacle"]["file_name"],
                    x * BlockSize, y * BlockSize,       // screen position
                    0,0,               // draw size
                    0,0                               // anchor top-left
                );
                spr->Size = TileSize;
                spr->SourceH = (float)MapState[y][x]["Obstacle"]["h"] - 2;
                spr->SourceW = (float)MapState[y][x]["Obstacle"]["w"] - 2;
                spr->SourceX = (float)MapState[y][x]["Obstacle"]["w"]  * (float)MapState[y][x]["Obstacle"]["x"] + 1;
                spr->SourceY = (float)MapState[y][x]["Obstacle"]["h"]  * (float)MapState[y][x]["Obstacle"]["y"] + 1;
                ObstacleGroup->AddNewObject(spr);
                Obstacle_dict[y + x*MapWidth] = spr;
            }
            // tree obstacle
            if(preview->id == 4){
                
                MapState[y][x]["Obstacle"]["x"] = rand()%(7) + 0 + Xgap*0;
                MapState[y][x]["Obstacle"]["y"] = rand()%(4) + rand()%2*(2.0/1.5 + 4);
                MapState[y][x]["Obstacle"]["w"] = tile["Tile"]["w"];
                MapState[y][x]["Obstacle"]["h"] = (float)tile["Tile"]["h"]*1.5;
                MapState[y][x]["Obstacle"]["file_name"] = "play/grass/vegetation.png";
                MapState[y][x]["Obstacle"]["Penetrable"] = false;
                MapState[y][x]["Obstacle"]["SizeX"] = 64;
                MapState[y][x]["Obstacle"]["SizeY"] = 96;
                MapState[y][x]["Obstacle"]["OffsetX"] = 0;
                MapState[y][x]["Obstacle"]["OffsetY"] = -32;
                if(Obstacle_dict.count(y + x*MapWidth))
                ObstacleGroup->RemoveObject(Obstacle_dict[y + x*MapWidth]->GetObjectIterator());
                auto* spr = new Engine::Sprite(
                    MapState[y][x]["Obstacle"]["file_name"],
                    x * BlockSize, y * BlockSize-(32),       // screen position
                    0,0,               // draw size
                    0,0                               // anchor top-left
                );
                spr->Size = Engine::Point(64,64*1.5);
                spr->SourceH = (float)MapState[y][x]["Obstacle"]["h"] - 2;
                spr->SourceW = (float)MapState[y][x]["Obstacle"]["w"] - 2;
                spr->SourceX = (float)MapState[y][x]["Obstacle"]["w"]  * (float)MapState[y][x]["Obstacle"]["x"] + 1;
                spr->SourceY = (float)MapState[y][x]["Obstacle"]["h"]  * (float)MapState[y][x]["Obstacle"]["y"] + 1;
                ObstacleGroup->AddNewObject(spr);
                Obstacle_dict[y + x*MapWidth] = spr;
            }
            // eraser
            if(preview->id == 5){
                MapState[y][x].erase("Obstacle");
                std::cerr<<"sdf "<<x<<' '<<y<<'\n';
                if(Obstacle_dict.count(y + x*MapWidth)){
                    std::cerr<<"obstacle eraser"<<x<<' '<<y<<'\n';
                    ObstacleGroup->RemoveObject(Obstacle_dict[y + x*MapWidth]->GetObjectIterator());
                    Obstacle_dict.erase(y + x*MapWidth);
                }
            }
            //enemy 0
            if(preview->id == 10){
                MapState[y][x]["Obstacle"]["x"] = 0;
                MapState[y][x]["Obstacle"]["y"] = 0;
                MapState[y][x]["Obstacle"]["file_name"] = "play/logo-nthu.png";
                MapState[y][x]["Obstacle"]["Penetrable"] = true;
                MapState[y][x]["Obstacle"]["SizeX"] = 64;
                MapState[y][x]["Obstacle"]["SizeY"] = 64;
                MapState[y][x]["SpawnPoint"] = 0;//spawn Enemy 0
                MapState[y][x]["SpawnCoolDown"] = 5; // if there is no enemy created by this tile, it will create a new enemy after X second
                
                if(Obstacle_dict.count(y + x*MapWidth))
                ObstacleGroup->RemoveObject(Obstacle_dict[y + x*MapWidth]->GetObjectIterator());
                auto* spr = new Engine::Sprite(
                    MapState[y][x]["Obstacle"]["file_name"],
                    x * BlockSize, y * BlockSize,       // screen position
                    0,0,               // draw size
                    0,0                               // anchor top-left
                );
                spr->Size = Engine::Point(64,64);
                MapState[y][x]["Obstacle"]["w"] = spr->GetBitmapWidth();
                MapState[y][x]["Obstacle"]["h"] = spr->GetBitmapHeight();
                ObstacleGroup->AddNewObject(spr);
                Obstacle_dict[y + x*MapWidth] = spr;
            }
            // enemy1
            if(preview->id == 11){
                MapState[y][x]["Obstacle"]["x"] = 0;
                MapState[y][x]["Obstacle"]["y"] = 0;
                MapState[y][x]["Obstacle"]["file_name"] = "play/logo-nthu.png";
                MapState[y][x]["Obstacle"]["Penetrable"] = true;
                MapState[y][x]["Obstacle"]["SizeX"] = 64;
                MapState[y][x]["Obstacle"]["SizeY"] = 64;
                MapState[y][x]["SpawnPoint"] = 1;//spawn Enemy 1
                MapState[y][x]["SpawnCoolDown"] = 5; // if there is no enemy created by this tile, it will create a new enemy after X second
                
                if(Obstacle_dict.count(y + x*MapWidth))
                ObstacleGroup->RemoveObject(Obstacle_dict[y + x*MapWidth]->GetObjectIterator());
                auto* spr = new Engine::Sprite(
                    MapState[y][x]["Obstacle"]["file_name"],
                    x * BlockSize, y * BlockSize,       // screen position
                    0,0,               // draw size
                    0,0                               // anchor top-left
                );
                spr->Size = Engine::Point(64,64);
                MapState[y][x]["Obstacle"]["w"] = spr->GetBitmapWidth();
                MapState[y][x]["Obstacle"]["h"] = spr->GetBitmapHeight();
                ObstacleGroup->AddNewObject(spr);
                Obstacle_dict[y + x*MapWidth] = spr;
            }
            // enemy2
            if(preview->id == 12){
                MapState[y][x]["Obstacle"]["x"] = 0;
                MapState[y][x]["Obstacle"]["y"] = 0;
                MapState[y][x]["Obstacle"]["file_name"] = "play/logo-nthu.png";
                MapState[y][x]["Obstacle"]["Penetrable"] = true;
                MapState[y][x]["Obstacle"]["SizeX"] = 64;
                MapState[y][x]["Obstacle"]["SizeY"] = 64;
                MapState[y][x]["SpawnPoint"] = 2;//spawn Enemy 2
                MapState[y][x]["SpawnCoolDown"] = 5; // if there is no enemy created by this tile, it will create a new enemy after X second
                
                if(Obstacle_dict.count(y + x*MapWidth))
                ObstacleGroup->RemoveObject(Obstacle_dict[y + x*MapWidth]->GetObjectIterator());
                auto* spr = new Engine::Sprite(
                    MapState[y][x]["Obstacle"]["file_name"],
                    x * BlockSize, y * BlockSize,       // screen position
                    0,0,               // draw size
                    0,0                               // anchor top-left
                );
                spr->Size = Engine::Point(64,64);
                MapState[y][x]["Obstacle"]["w"] = spr->GetBitmapWidth();
                MapState[y][x]["Obstacle"]["h"] = spr->GetBitmapHeight();
                ObstacleGroup->AddNewObject(spr);
                Obstacle_dict[y + x*MapWidth] = spr;
            }
        }
    }
}
void DrawMapScene::OnKeyDown(int keyCode)  {
    IScene::OnKeyDown(keyCode);
    for (auto n: PlayerGroup->GetObjects()) {
        Player* player = dynamic_cast<Player*>(n);
        player->OnKeyDown(keyCode);
    }
}
void DrawMapScene::OnKeyUp(int keyCode) {
    for (auto n: PlayerGroup->GetObjects()) {
        Player* player = dynamic_cast<Player*>(n);
        player->OnKeyUp(keyCode);
    }

}

void DrawMapScene::ReadMap(){
    std::cerr<<"DrawMapScene!!\n";

    for (int i = 0; i < MapHeight; ++i) {
    for (int j = 0; j < MapWidth; ++j) {
        int linearIndex = j + i * MapWidth;
        std::string key = std::to_string(linearIndex);

        auto& tile = MapState[i][j]["Tile"];
        auto* spr = new Engine::Sprite(
            tile["file_name"],
            j * BlockSize, i * BlockSize,
            0, 0,
            0, 0
        );
        spr->Size = TileSize;
        spr->SourceH = (float)tile["h"] - 2;
        spr->SourceW = (float)tile["w"] - 2;
        spr->SourceX = (float)tile["w"] * (float)tile["x"] + 1;
        spr->SourceY = (float)tile["h"] * (float)tile["y"] + 1;
        TileMapGroup->AddNewObject(spr);
        Tile_dict[linearIndex] = spr;

        // Obstacle
        if (MapState[i][j].contains("Obstacle")) {
            auto& obs = MapState[i][j]["Obstacle"];

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
                //eraser
                std::cerr<<ox<<' '<<oy<<'\n';
                Obstacle_dict[i + j*MapWidth] = obs_spr;
                if (obs.contains("SizeX") && obs.contains("SizeY")) {
                    obs_spr->Size.x = obs["SizeX"];
                    obs_spr->Size.y = obs["SizeY"];
                }

                // offset
                
                 {
                    std::cerr<<"hello obstacle\n";
                    auto& innerObs = obs;
                    if (innerObs.contains("OffsetX")) {
                        obs_spr->Position.x += (float)innerObs["OffsetX"];
                        std::cerr<<"hello offset X "<<(float)innerObs["OffsetX"]<<'\n';
                    }
                    if (innerObs.contains("OffsetY")) {
                        obs_spr->Position.y += (float)innerObs["OffsetY"];
                        std::cerr<<"hello offsetY "<<(float)innerObs["OffsetY"]<<'\n';
                    }
                }

                ObstacleGroup->AddNewObject(obs_spr);
                
            }
        }
    }
}

}

void DrawMapScene::ConstructUI() {
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    Engine::Image * sand = new Engine::Image("play/sand.png", 1280, 0, 320, 832);
    sand->fixed = true;
    UIGroup->AddNewObject(sand);

    {
        TextureButton *btn;
        //texture 1 grass
        btn = new TextureButton("play/floor.png", "play/dirt.png",
                            Engine::Sprite("play/grass/grounds.png", 1294 + 5, 80 + 5, 0, 0, 0, 0), 1294, 80);
        btn->TileTexture.Size = Engine::Point(54,54);
        btn->TileTexture.SourceH = tileH;
        btn->TileTexture.SourceW = tileW;
        btn->TileTexture.SourceX = (7 + Xgap*0) * tileW;
        btn->TileTexture.SourceY = (0 + Ygap*0) * tileH;
        btn->fixed = true;
        // Reference: Class Member Function Pointer and std::bind.
        // UIBtnClicked(0);
        btn->SetOnClickCallback(std::bind(&DrawMapScene::UIBtnClicked, this, 0));
        UIGroup->AddNewControlObject(btn);

        // texture 2 sand
        btn = new TextureButton("play/floor.png", "play/dirt.png",
                            Engine::Sprite("play/grass/grounds.png", 1294 + 5 + 100, 80 + 5, 0, 0, 0, 0), 1294 + 100, 80);
        btn->TileTexture.Size = Engine::Point(54,54);
        btn->TileTexture.SourceH = tileH;
        btn->TileTexture.SourceW = tileW;
        btn->TileTexture.SourceX = (7 + Xgap*1) * tileW;
        btn->TileTexture.SourceY = (0 + Ygap*0) * tileH;
        btn->fixed = true;
        // Reference: Class Member Function Pointer and std::bind.
        // UIBtnClicked(0);
        btn->SetOnClickCallback(std::bind(&DrawMapScene::UIBtnClicked, this, 1));
        UIGroup->AddNewControlObject(btn);

        // texture 3 stone
        btn = new TextureButton("play/floor.png", "play/dirt.png",
                            Engine::Sprite("play/grass/grounds.png", 1294 + 5 + 200, 80 + 5, 0, 0, 0, 0), 1294 + 200, 80);
        btn->TileTexture.Size = Engine::Point(54,54);
        btn->TileTexture.SourceH = tileH;
        btn->TileTexture.SourceW = tileW;
        btn->TileTexture.SourceX = (7 + Xgap*1) * tileW;
        btn->TileTexture.SourceY = (0 + Ygap*1) * tileH;
        btn->fixed = true;
        // Reference: Class Member Function Pointer and std::bind.
        // UIBtnClicked(0);
        btn->SetOnClickCallback(std::bind(&DrawMapScene::UIBtnClicked, this, 2));
        UIGroup->AddNewControlObject(btn);
        
        // texture 4 stone -> can't penetrate
        btn = new TextureButton("play/floor.png", "play/dirt.png",
                            Engine::Sprite("play/grass/terrain.png", 1294 + 5 + 0, 80 + 5 + 100, 0, 0, 0, 0), 1294 + 0, 80 + 100);
        btn->TileTexture.Size = Engine::Point(54,54);
        btn->TileTexture.SourceH = tileH;
        btn->TileTexture.SourceW = tileW;
        btn->TileTexture.SourceX = (1 + Xgap*1) * tileW;
        btn->TileTexture.SourceY = (0 + Ygap*0) * tileH;
        btn->fixed = true;
        // Reference: Class Member Function Pointer and std::bind.
        // UIBtnClicked(0);
        btn->SetOnClickCallback(std::bind(&DrawMapScene::UIBtnClicked, this, 3));
        UIGroup->AddNewControlObject(btn);

        // texture 5 tree -> can't penetrate
        btn = new TextureButton("play/floor.png", "play/dirt.png",
                            Engine::Sprite("play/grass/vegetation.png", 1294 + 5 + 0 + 100, 80 + 5 + 100, 0, 0, 0, 0), 1294 + 0 + 100, 80 + 100);
        btn->TileTexture.Size = Engine::Point(54,54);
        btn->TileTexture.SourceH = tileH;
        btn->TileTexture.SourceW = tileW;
        btn->TileTexture.SourceX = (0 + Xgap*0) * tileW;
        btn->TileTexture.SourceY = (0 + Ygap*0) * tileH;
        btn->fixed = true;
        // Reference: Class Member Function Pointer and std::bind.
        // UIBtnClicked(0);
        btn->SetOnClickCallback(std::bind(&DrawMapScene::UIBtnClicked, this, 4));
        UIGroup->AddNewControlObject(btn);

        // texture 6 eraser
        btn = new TextureButton("play/floor.png", "play/dirt.png",
                            Engine::Sprite("play/eraser.png", 1294 + 5 + 0 + 200, 80 + 5 + 100, 0, 0, 0, 0), 1294 + 0 + 200, 80 + 100);
        btn->TileTexture.Size = Engine::Point(54,54);
        btn->fixed = true;
        // Reference: Class Member Function Pointer and std::bind.
        // UIBtnClicked(0);
        btn->SetOnClickCallback(std::bind(&DrawMapScene::UIBtnClicked, this, 5));
        UIGroup->AddNewControlObject(btn);
    }

    {
        TextureButton *btn;
        // spawnpoint 0
        btn = new TextureButton("play/floor.png", "play/dirt.png",
                            Engine::Sprite("play/logo-nthu.png", 1294 + 5 + 0 + 0, 80 + 5 + 300, 0, 0, 0, 0), 1294 + 0 + 0, 80 + 300);
        btn->TileTexture.Size = Engine::Point(54,54);
        btn->fixed = true;
        btn->SetOnClickCallback(std::bind(&DrawMapScene::UIBtnClicked, this, 10));
        UIGroup->AddNewControlObject(btn);

        // spawnpoint 1
        btn = new TextureButton("play/floor.png", "play/dirt.png",
                            Engine::Sprite("play/logo-nthu.png", 1294 + 5 + 0 + 100, 80 + 5 + 300, 0, 0, 0, 0), 1294 + 0 + 100, 80 + 300);
        btn->TileTexture.Size = Engine::Point(54,54);
        btn->fixed = true;
        btn->SetOnClickCallback(std::bind(&DrawMapScene::UIBtnClicked, this, 11));
        UIGroup->AddNewControlObject(btn);
        
        // spawnpoint 2
        btn = new TextureButton("play/floor.png", "play/dirt.png",
                            Engine::Sprite("play/logo-nthu.png", 1294 + 5 + 0 + 200, 80 + 5 + 300, 0, 0, 0, 0), 1294 + 0 + 200, 80 + 300);
        btn->TileTexture.Size = Engine::Point(54,54);
        btn->fixed = true;
        btn->SetOnClickCallback(std::bind(&DrawMapScene::UIBtnClicked, this, 12));
        UIGroup->AddNewControlObject(btn);
    }

    {
        //Save button
        Engine::ImageButton *btn;
        Engine::Label *lab;
        btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", 1294, 750, 100, 50);
        btn->fixed = true;
        btn->SetOnClickCallback(std::bind(&DrawMapScene::UIBtnClicked, this, -2));
        UIGroup->AddNewControlObject(btn);
        lab = new Engine::Label("Save", "pirulen.ttf", 24, 1294+50, 750 + 25, 0, 0, 0, 255, 0.5, 0.5);
        lab->fixed = true;
        UIGroup->AddNewObject(lab);

        //Save button to Map1
        btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", game.screenW/4-100, game.screenH/2-50, 200, 100);
        btn->fixed = true;
        btn->SetOnClickCallback(std::bind(&DrawMapScene::UIBtnClicked, this, -3));
        LabelGroup->AddNewControlObject(btn);
        lab = new Engine::Label("Map1", "pirulen.ttf", 48, game.screenW/4+70-100 - 60, game.screenH/2 + 25-50, 0, 0, 0, 255);
        lab->fixed = true;
        btn->Visible = false; btn->Enabled = false; lab->Visible = false;
        LabelGroup->AddNewObject(lab);
        SelectFile[0] = std::make_pair(btn,lab);

        //Save button to Map2
        btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", game.screenW/4*2-100, game.screenH/2-50, 200, 100);
        btn->fixed = true;
        btn->SetOnClickCallback(std::bind(&DrawMapScene::UIBtnClicked, this, -4));
        LabelGroup->AddNewControlObject(btn);
        lab = new Engine::Label("Map2", "pirulen.ttf", 48, game.screenW/4*2+70-100 - 60, game.screenH/2 + 25-50, 0, 0, 0, 255);
        lab->fixed = true;
        btn->Visible = false; btn->Enabled = false; lab->Visible = false;
        LabelGroup->AddNewObject(lab);
        SelectFile[1] = std::make_pair(btn,lab);

        //Save button to Map3
        btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", game.screenW/4*3 - 100, game.screenH/2 - 50, 200, 100);
        btn->fixed = true;
        btn->SetOnClickCallback(std::bind(&DrawMapScene::UIBtnClicked, this, -5));
        LabelGroup->AddNewControlObject(btn);
        lab = new Engine::Label("Map3", "pirulen.ttf", 48, game.screenW/4*3+70 - 100  - 70, game.screenH/2 + 25 -50, 0, 0, 0, 255);
        lab->fixed = true;
        btn->Visible = false; btn->Enabled = false; lab->Visible = false;
        LabelGroup->AddNewObject(lab);
        SelectFile[2] = std::make_pair(btn,lab);

        

        //leave button
        btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", 1294 + 150, 750, 100, 50);
        btn->fixed = true;
        btn->SetOnClickCallback(std::bind(&DrawMapScene::UIBtnClicked, this, -100));
        UIGroup->AddNewControlObject(btn);
        lab = new Engine::Label("Exit", "pirulen.ttf", 24, 1294+50 + 150, 750 + 25, 0, 0, 0, 255, 0.5, 0.5);
        lab->fixed = true;
        UIGroup->AddNewObject(lab);
    }
}

void DrawMapScene::UIBtnClicked(int id){

    if(preview) {
        UIGroup->RemoveObject(preview->GetObjectIterator());
    }
    if(id == -1){
        UIGroup->RemoveObject(preview->GetObjectIterator());
        preview = nullptr;
    }
    if(id == 0) {
        preview = new Engine::Sprite("play/grass/grounds.png", 100, 100, 0, 0, 0.5, 0.5, 0, 0, 0, 255, 255, 255, 50);
        preview->id = 0;
        preview->Size = TileSize;
        preview->SourceH = tileH;
        preview->SourceW = tileW;
        preview->SourceX = (7 + Xgap*0) * tileH;
        preview->SourceY = (0 + Ygap*0) * tileW;
    }
    if(id == 1){
        preview = new Engine::Sprite("play/grass/grounds.png", 100, 100, 0, 0, 0.5, 0.5, 0, 0, 0, 255, 255, 255, 50);
        preview->id = 1;
        preview->Size = TileSize;
        preview->SourceH = tileH;
        preview->SourceW = tileW;
        preview->SourceX = (7 + Xgap*1) * tileH;
        preview->SourceY = (0 + Ygap*0) * tileW;
    }
    if(id == 2){
        preview = new Engine::Sprite("play/grass/grounds.png", 100, 100, 0, 0, 0.5, 0.5, 0, 0, 0, 255, 255, 255, 50);
        preview->id = 2;
        preview->Size = TileSize;
        preview->SourceH = tileH;
        preview->SourceW = tileW;
        preview->SourceX = (7 + Xgap*1) * tileH;
        preview->SourceY = (0 + Ygap*1) * tileW;
    }
    if(id == 3){
        preview = new Engine::Sprite("play/grass/terrain.png", 100, 100, 0, 0, 0.5, 0.5, 0, 0, 0, 255, 255, 255, 50);
        preview->id = 3;
        preview->Size = TileSize;
        preview->SourceH = tileH;
        preview->SourceW = tileW;
        preview->SourceX = (1 + Xgap*1) * tileH;
        preview->SourceY = (0 + Ygap*0) * tileW;
    }
    if(id == 4){
        preview = new Engine::Sprite("play/grass/vegetation.png", 100, 100, 0, 0, 0.5, 0.5, 0, 0, 0, 255, 255, 255, 50);
        preview->id = 4;
        preview->Size = TileSize;
        preview->SourceH = tileH;
        preview->SourceW = tileW;
        preview->SourceX = (0 + Xgap*0) * tileH;
        preview->SourceY = (0 + Ygap*0) * tileW;
    }
    if(id == 5){
        std::cerr<<"id 5\n";
        preview = new Engine::Sprite("play/eraser.png", 100, 100, 0, 0, 0.5, 0.5, 0, 0, 0, 255, 255, 255, 50);
        preview->id = 5;
        preview->Size = TileSize;
    }
    if(id == 10){
        std::cerr<<"id 10\n";
        preview = new Engine::Sprite("play/logo-nthu.png", 100, 100, 0, 0, 0.5, 0.5, 0, 0, 0, 255, 255, 255, 50);
        preview->id = 10;
        preview->Size = TileSize;
    }
    if(id == 11){
        std::cerr<<"id 11\n";
        preview = new Engine::Sprite("play/logo-nthu.png", 100, 100, 0, 0, 0.5, 0.5, 0, 0, 0, 255, 255, 255, 50);
        preview->id = 11;
        preview->Size = TileSize;
    }
    if(id == 12){
        std::cerr<<"id 12\n";
        preview = new Engine::Sprite("play/logo-nthu.png", 100, 100, 0, 0, 0.5, 0.5, 0, 0, 0, 255, 255, 255, 50);
        preview->id = 12;
        preview->Size = TileSize;
    }
    if(id == -2){
        // SaveMapStateToFile("Resource/map2.json");
        pause = true;
        for(int i = 0; i < 3; i++){
            SelectFile[i].first->Visible = true;
            SelectFile[i].second->Visible = true;
            SelectFile[i].first->Enabled = true;
        }
    }
    if(id == -3){
        SaveMapStateToFile("Resource/map1.json");
        // Engine::GameEngine::GetInstance().ChangeScene("stage-select");
    }
    if(id == -4){
        SaveMapStateToFile("Resource/map2.json");
        // Engine::GameEngine::GetInstance().ChangeScene("stage-select");
    }
    if(id == -5){
        SaveMapStateToFile("Resource/map3.json");
        // Engine::GameEngine::GetInstance().ChangeScene("stage-select");
    }
    if(id == -100){
        Engine::GameEngine::GetInstance().ChangeScene("stage-select");
    }
    if(!preview)
        return;
    preview->fixed = true;
    preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
    preview->Tint = al_map_rgba(255, 255, 255, 200);
    UIGroup->AddNewObject(preview);
    
}


void DrawMapScene::SaveMapStateToFile(const std::string& path) {
    if(save_cooldown > 0)return;
    save_cooldown = 5.0;
    nlohmann::json output;
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    output["MapWidth"] = MapWidth;
    output["MapHeight"] = MapHeight;
    output["MapState"] = MapState; 

    std::ofstream fout(path);
    if (!fout.is_open()) {
        std::cerr << "Failed to open file for writing: " << path << '\n';
        return;
    }
    fout << output.dump(4); // 4 = pretty print
    std::cerr<<output.dump(4)<<'\n';
    fout.close();
    std::cerr << "MapState saved to " << path << '\n';
    // PlayerGroup->AddNewObject(new DirtyEffect("play/correct.png", 3, game.screenW, game.screenH));
    for(int i = 0; i < 3; i++){
        SelectFile[i].first->Visible = false;
        SelectFile[i].first->Enabled = false;
        SelectFile[i].second->Visible = false;
    }
    pause = 0;
    correct_time = 3.0f;
    if(correct != nullptr){
        correct->Visible = true;
    }else{
        std::cerr<<"shit\n";
    }
    
}

void DrawMapScene::RenderVisibleTiles() const {
    auto visibleArea = camera->GetVisibleTileArea(BlockSize);
    
   
    for (auto obj : TileMapGroup->GetObjects()) {
        Engine::Sprite* sprite = dynamic_cast<Engine::Sprite*>(obj);
        if (!sprite) continue;
        
       
        if (camera->IsInView(sprite->Position, BlockSize)) {
            
            Engine::Point screenPos = camera->WorldToScreen(sprite->Position);
            

            Engine::Point originalPos = sprite->Position;
            sprite->Position = screenPos;
            sprite->Draw();
            sprite->Position = originalPos;
        }
    }
}

void DrawMapScene::RenderVisibleObjects() const {
    std::vector<Group*> renderGroups = {
         PlayerGroup,ObstacleGroup,UIGroup
    };
    
    for (Group* group : renderGroups) {
        for (auto obj : group->GetObjects()) {
            if(!obj->Visible)continue;
            if(obj->fixed){
                obj->Draw();
                continue;
            }
            if (camera->IsInView(obj->Position, 64)) { 
                Engine::Point screenPos = camera->WorldToScreen(obj->Position);
                Engine::Point originalPos = obj->Position;
                obj->Position = screenPos;
                obj->Draw();
                obj->Position = originalPos;
            }
        }
    }
    
    GroundEffectGroup->Draw();
    // if press save -> draw button and block scene
    if(pause)
    al_draw_filled_rectangle(0, 0,
        Engine::GameEngine::GetInstance().screenW,
        Engine::GameEngine::GetInstance().screenH,
        al_map_rgba(0, 0, 0, 128));

    for (auto obj : LabelGroup->GetObjects()) {
            if(!obj->Visible)continue;
            if(obj->fixed){
                obj->Draw();
                continue;
            }
            if (camera->IsInView(obj->Position, 64)) { 
                Engine::Point screenPos = camera->WorldToScreen(obj->Position);
                Engine::Point originalPos = obj->Position;
                obj->Position = screenPos;
                obj->Draw();
                obj->Position = originalPos;
            }
    }
    
}