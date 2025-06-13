#include "LoadingScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Connect/Client.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Component/Label.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

static std::string Texts[4] = {"Loading","Loading.","Loading..","Loading..."};
static std::string Images[2] = {"loading/hutao1.png","loading/hutao2.png"};

void LoadingScene::Initialize() {
    mapSent = false;
    ticks = 0;
    accumulatedTime = 0.0f; // initial time
    AddNewControlObject(UIGroup = new Group());
    dot_cooldown = 0.5f;
    ImageLoading.resize(2);
    ConstructUI();
    weaponSelected = std::vector<bool>(4, false);
}

void LoadingScene::Update(float deltaTime) {
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    GameClient &client = game.GetSender();
    ticks += deltaTime;
    accumulatedTime += deltaTime; 
    // every 2s send map
    if(mapReceived == false)
    if (accumulatedTime >= 0.5f) {
        accumulatedTime = 0.0f;

        // Read Resource/map2.json
        std::string filename = "Resource/map" + std::to_string(MapNum) + ".json";
        std::fstream fin(filename);
        if (fin.is_open()) {
            nlohmann::json mapJson;
            fin >> mapJson;
            // Send map in correct format as JSON object
            client.output_json = {
                {"map", mapJson}
            };
        } else {
            std::cerr << "Failed to open Resource/map2.json\n";
        }
        std::cerr<<"request "<<filename<<'\n';
    }
    // Wait until map is received back from server
    if (client.input_json.contains("map")) {
        std::cerr<<"GetMap!\n";
        const nlohmann::json& receivedMap = client.input_json["map"];
        std::string filename = "Resource/loadingMap.json";
        std::ofstream fout(filename);
        if (fout.is_open()) {
            fout << receivedMap["map"].dump(4); // Indented output
            std::cerr<<"hello Map "<< receivedMap.dump()<<'\n';
            fout.close();
        } else {
            std::cerr << "Failed to write to Resource/map1.json\n";
        }

        // Send acknowledgment tag to stop server from sending map repeatedly
        client.output_json = {
            {"Tag", "MapReceived"}
        };

        // Switch to PlayScene
        std::cerr<<"CHANGE!\n";
        mapReceived = true;
    }

    //UI
    // text Loading...
    dot_cooldown -= deltaTime;
    if(dot_cooldown < 0){
        dot_num = (dot_num + 1) % 4;
        TextLoading->Text = Texts[dot_num];
        int t = rand()%2;
        ImageLoading[0]->Visible = (t==0);
        ImageLoading[1]->Visible = (t==1); 
        dot_cooldown = 0.5f;
    }

    UIGroup->Update(deltaTime);
}

void LoadingScene::ConstructUI(){
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();

    // --- Loading Text (Top) ---
    TextLoading = new Engine::Label("Loading", "pirulen.ttf", 48, game.screenW / 2, 50, 255, 255, 255, 255, 0.5, 0.5);
    UIGroup->AddNewObject(TextLoading);

    // --- Weapon Buttons (2 rows x 2) ---
    float btnSize = 96;
    float startX = game.screenW / 2 - btnSize - 40;
    float startY = 150;
    WeaponButtons.resize(4);
    weaponSelected = std::vector<bool>(4, false);
    selectedCount = 0;

    for (int i = 0; i < 4; ++i) {
        float x = startX + (i % 2) * (btnSize + 80);
        float y = startY + (i / 2) * (btnSize + 80);
        std::string label = "Weapon " + std::to_string(i + 1);

        Engine::ImageButton* btn = new Engine::ImageButton(
            "stage-select/dirt.png", "stage-select/floor.png",
            x, y, btnSize, btnSize
        );
        btn->SetOnClickCallback([this, i]() { OnClickWeapon(i); });
        WeaponButtons[i] = btn;
        UIGroup->AddNewControlObject(btn);

        Engine::Label* weaponLabel = new Engine::Label(
            label, "pirulen.ttf", 20,
            x + btnSize / 2, y - 20, 255, 255, 255, 255, 0.5, 0.5
        );
        UIGroup->AddNewObject(weaponLabel);
    }

    // --- Confirm Button ---
    ConfirmButton = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", game.screenW / 2 - 100, startY + 2 * (btnSize + 80), 200, 64);
    ConfirmButton->SetOnClickCallback(std::bind(&LoadingScene::OnClickConfirm, this));
    UIGroup->AddNewControlObject(ConfirmButton);

    Engine::Label* confirmLabel = new Engine::Label("Confirm", "pirulen.ttf", 24, game.screenW / 2, startY + 2 * (btnSize + 80) + 32, 255, 255, 255, 255, 0.5, 0.5);
    UIGroup->AddNewObject(confirmLabel);

    // --- Hutao Animation (bottom)
    ImageLoading[0] = new Engine::Sprite(Images[0], game.screenW / 2 - 64, game.screenH - 96, 64, 64);
    ImageLoading[1] = new Engine::Sprite(Images[1], game.screenW / 2 - 64, game.screenH - 96, 64, 64);
    UIGroup->AddNewObject(ImageLoading[0]);
    UIGroup->AddNewObject(ImageLoading[1]);

}

void LoadingScene::OnClickWeapon(int index) {
    if (weaponSelected[index]) {
        weaponSelected[index] = false;
        selectedCount--;
    } else {
        if (selectedCount >= 2) return;
        weaponSelected[index] = true;
        selectedCount++;
    }
    
}

void LoadingScene::OnClickConfirm() {
    if (selectedCount >= 1 && mapReceived) {
        auto* play = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play"));
        if (play) {
            play->PlayerWeapon.clear();
            for (int i = 0; i < 4; ++i) {
                if (weaponSelected[i]) {
                    play->PlayerWeapon.insert(i);
                }
            }
        }
        Engine::GameEngine::GetInstance().ChangeScene("play");
    }

}
