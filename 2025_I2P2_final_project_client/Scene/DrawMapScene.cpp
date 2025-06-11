#define NOMINMAX
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

#include "DrawMapScene.hpp"
#include "Player/Player.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Resources.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Component/Label.hpp"
#include "Map/Texture.hpp"
#include "Map/TextureButton.hpp"

const int DrawMapScene::MapWidth = 30, DrawMapScene::MapHeight = 30;
const int DrawMapScene::BlockSize = 64;
void DrawMapScene::Initialize() {
    AddNewObject(TileMapGroup = new Group());
    AddNewControlObject(UIGroup = new Group());
    AddNewObject(PlayerGroup = new Group());
    {
        Engine::GameEngine &game = Engine::GameEngine::GetInstance();
        Player* newPlayer = new Player(100, 100, game.my_id);
        PlayerGroup->AddNewObject(newPlayer);
        player_dict[game.my_id] = newPlayer;
    }

    //DrawUI
    preview = nullptr;
    imgTarget = new Engine::Image("play/target.png", 0, 0);
    imgTarget->Visible = false;
    ConstructUI();

    //Map
    nlohmann::json tile = { {"Tile", "G"} };
    MapState.resize(MapHeight, std::vector<nlohmann::json>(MapWidth,tile));
    ReadMap();

}

void DrawMapScene::Terminate() {
    IScene::Terminate();
}
void DrawMapScene::Update(float deltaTime) {
    // update client's W, A, S, DAdd commentMore actions
    // PlayerGroup->Update(deltaTime);

    // // check new player join & update all players' position
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    GameClient &sender = game.GetSender();
    
    player_dict[game.my_id]->UpdateMyPlayer(deltaTime);
    IScene::Update(deltaTime);

    // preview
    if (preview) {
        preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
        // To keep responding when paused.
        preview->Update(deltaTime);
    }
}
void DrawMapScene::Draw() const {
    IScene::Draw();
}
void DrawMapScene::OnMouseDown(int button, int mx, int my) {
    IScene::OnMouseDown(button, mx, my);
}
void DrawMapScene::OnMouseMove(int mx, int my) {
    IScene::OnMouseMove(mx, my);
    const int x = mx / BlockSize;
    const int y = my / BlockSize;
    if (!preview || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight) {
        imgTarget->Visible = false;
        return;
    }
    imgTarget->Visible = true;
    imgTarget->Position.x = x * BlockSize;
    imgTarget->Position.y = y * BlockSize;
}

void DrawMapScene::OnMouseUp(int button, int mx, int my) {
    IScene::OnMouseUp(button, mx, my);
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
    std::vector<std::vector<int>>mapState_2(MapHeight, std::vector<int>(MapWidth));
    for (int i = 0; i < MapHeight; ++i) {
        for (int j = 0; j < MapWidth; ++j) {
            if (!MapState[i][j].contains("Tile")) {
                std::cerr << "Missing tile at index " << i<<' '<<j << "\n";
                nlohmann::json tile = { {"Tile", "G"} };
                MapState[i][j] = tile;
                continue;
            }
            mapState_2[i][j] = (MapState[i][j]["Tile"] == "G")? 0: 1;
        }
    }

    for (int i = 0; i < MapHeight; ++i) {
        for (int j = 0; j < MapWidth; ++j) {
            int linearIndex = i + j * MapHeight;
            std::string key = std::to_string(linearIndex);
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
    }
}

void DrawMapScene::ConstructUI() {
    UIGroup->AddNewObject(new Engine::Image("play/sand.png", 1280, 0, 320, 832));

    TextureButton *btn;
    btn = new TextureButton("play/floor.png", "play/dirt.png",
                           Engine::Sprite("play/grass/03.png", 1294 + 5, 80 + 5, BlockSize-10, BlockSize-10, 0, 0), 1294, 80);
    // Reference: Class Member Function Pointer and std::bind.
    // UIBtnClicked(0);
    btn->SetOnClickCallback(std::bind(&DrawMapScene::UIBtnClicked, this, 0));
    UIGroup->AddNewControlObject(btn);
}

void DrawMapScene::UIBtnClicked(int id){
    if(preview) {
        UIGroup->RemoveObject(preview->GetObjectIterator());
    }
    if(id == 0) {
        preview = new Engine::Sprite("play/grass/03.png", 100, 100, BlockSize, BlockSize);
    }


    if(!preview)
        return;

    preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
    preview->Tint = al_map_rgba(255, 255, 255, 200);
    UIGroup->AddNewObject(preview);
    OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
}