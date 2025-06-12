#include "LoadingScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Connect/Client.hpp"
#include "Scene/PlayScene.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

void LoadingScene::Initialize() {
    mapSent = false;
    ticks = 0;
    accumulatedTime = 0.0f; // initial time
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    GameClient &client = game.GetSender();


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
        std::ifstream fin("Resource/map2.json");
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
    }
    // Wait until map is received back from server
    if (client.input_json.contains("map")) {
        std::cerr<<"getMap!\n";
        const nlohmann::json& receivedMap = client.input_json["map"];
        std::ofstream fout("Resource/map1.json");
        if (fout.is_open()) {
            fout << receivedMap["map"].dump(4); // Indented output
            
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
}