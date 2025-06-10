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


const int DrawMapScene::MapWidth = 30, DrawMapScene::MapHeight = 30;

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

    ConstructUI();
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
    for (auto [_id, client_info] : sender.input_json.items()) {
        if (_id == "my_id") continue;
        int id = std::stoi(_id);

        if (!client_info.contains("player") || !client_info["player"].is_array() || client_info["player"].size() < 2)
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
    player_dict[game.my_id]->UpdateMyPlayer(deltaTime);
    IScene::Update(deltaTime);
}
void DrawMapScene::Draw() const {
    IScene::Draw();
}
void DrawMapScene::OnMouseDown(int button, int mx, int my) {
    IScene::OnMouseMove(mx, my);
}
void DrawMapScene::OnMouseMove(int mx, int my) {
    IScene::OnMouseMove(mx, my);
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

void DrawMapScene::ConstructUI() {

}