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
    AddNewObject(UIGroup = new Group());
    dot_cooldown = 0.5f;
    ImageLoading.resize(2);
    ConstructUI();
}

void LoadingScene::Update(float deltaTime) {
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    GameClient &client = game.GetSender();
    ticks += deltaTime;
    accumulatedTime += deltaTime; 
    // every 2s send map
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
        
        const nlohmann::json& receivedMap = client.input_json["map"];
        std::string filename = "loadingMap.json";
        std::ofstream fout(filename);
        if (fout.is_open()) {
            fout << receivedMap["map"].dump(4); // Indented output
            std::cerr<<"hello Map "<< filename<<'\n';
            fout.close();
        } else {
            std::cerr << "Failed to write to Resource/map1.json\n";
        }

        // Send acknowledgment tag to stop server from sending map repeatedly
        client.output_json = {
            {"Tag", "MapReceived"}
        };

        // Switch to PlayScene
        game.ChangeScene("play");
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

    
}

void LoadingScene::ConstructUI(){
    Engine::Label *lab;
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    // text Loading
    lab = new Engine::Label("Loading", "pirulen.ttf", 56, game.screenW/2, game.screenH/2, 255, 255, 255, 255, 0.5, 0.5);
    UIGroup->AddNewObject(lab);
    TextLoading = lab;
    
    Engine::Sprite *img;
    img = new Engine::Sprite(Images[0],  game.screenW/2, game.screenH/2+100,64,64);
    UIGroup->AddNewObject(img);
    ImageLoading[0] = img;

    img = new Engine::Sprite(Images[1],  game.screenW/2, game.screenH/2+100,64,64);
    UIGroup->AddNewObject(img);
    ImageLoading[1] = img;
}