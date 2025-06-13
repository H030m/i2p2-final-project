#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "settingWHScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
void SettingWHScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    W = 20, H = 20;
    Engine::ImageButton* btn;

    // Back 
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 300, halfH / 2 + 500, 600, 100);
    btn->SetOnClickCallback(std::bind(&SettingWHScene::BackOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("BACK", "pirulen.ttf", 48, halfW, halfH / 2 + 550, 0, 0, 0, 255, 0.5, 0.5));

    // H, W Label
    AddNewObject(new Engine::Label("H:", "pirulen.ttf", 48, 150, 150, 255, 255, 255, 255, 0, 0));
    AddNewObject(new Engine::Label("W:", "pirulen.ttf", 48, 150, 300, 255, 255, 255, 255, 0, 0));
    AddNewObject(TextH = new Engine::Label("50", "pirulen.ttf", 48, 250, 150, 255, 255, 255, 255, 0, 0));
    AddNewObject(TextW = new Engine::Label("50", "pirulen.ttf", 48, 250, 300, 255, 255, 255, 255, 0, 0));

    // Sliders
    AddNewControlObject(SliderH = new Slider(400, 150, 300, 4));
    AddNewControlObject(SliderW = new Slider(400, 300, 300, 4));
    SliderH->SetOnValueChangedCallback([](float) {});
    SliderW->SetOnValueChangedCallback([](float) {});
    SliderH->SetValue(0);
    SliderW->SetValue(0);
    // Confirm Button
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 300, halfH / 2 + 400, 600, 100);
    btn->SetOnClickCallback(std::bind(&SettingWHScene::GenerateMapAndEnter, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("NEXT", "pirulen.ttf", 48, halfW, halfH / 2 + 450, 0, 0, 0, 255, 0.5, 0.5));
}
void SettingWHScene::Update(float deltaTime) {
    IScene::Update(deltaTime);

    H = 20 + std::round(SliderH->GetValue() * 30);
    W = 20 + std::round(SliderW->GetValue() * 30);
    TextH->Text = std::to_string(H);
    TextW->Text = std::to_string(W);
}
void SettingWHScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
}

void SettingWHScene::BackOnClick(int stage){
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
void SettingWHScene::GenerateMapAndEnter() {
    using json = nlohmann::json;
    json map_json;
    std::cerr<<"New Map "<<H<<' '<<W<<'\n';
    map_json["MapHeight"] = H;
    map_json["MapWidth"] = W;
    map_json["MapState"] = json::array();

    for (int i = 0; i < H; ++i) {
        json row = json::array();
        for (int j = 0; j < W; ++j) {
            json cell;
            cell["Tile"] = {
                {"file_name", "play/grass/grounds.png"},
                {"h", 32},
                {"w", 32},
                {"x", 7},
                {"y", 0}
            };
            row.push_back(cell);
        }
        map_json["MapState"].push_back(row);
    }

    std::ofstream out("Resource/map5.json");
    if (out.is_open()) {
        out << map_json.dump(4); 
        std::cerr<<map_json.dump()<<'\n';
        out.close();
        Engine::GameEngine::GetInstance().ChangeScene("DrawMapScene");
    } else {
        std::cerr << "[ERROR] Failed to write map5.json\n";
    }
}