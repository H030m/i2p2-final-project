#include <functional>
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "PlayScene.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "SubmitScene.hpp"


void SubmitScene::Initialize() {
    // grade
    CulGrade();

    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    
    // Cancel Button
    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 450, halfH * 7 / 4 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&SubmitScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Cancel", "pirulen.ttf", 48, halfW - 250, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));
    
    //switch to submit Scene
    btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW + 50, halfH * 7 / 4 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&SubmitScene::SubmitOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Submit", "pirulen.ttf", 48, halfW + 250, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));

    // score label
    AddNewObject(new Engine::Label("SCORE: " + std::to_string(score) + " " + std::to_string(EndTime - StartTime), "pirulen.ttf", 64, halfW, 170, 255, 215, 0, 255, 0.5, 0.5));

    // input your name
    AddNewObject(new Engine::Label("Your Name", "pirulen.ttf", 64, halfW, 350, 255, 255, 255, 255, 0.5, 0.5));
    NameLabel = new Engine::Label("", "pirulen.ttf", 40, halfW, 450, 255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(NameLabel);
}
void SubmitScene::Terminate() {
    IScene::Terminate();
}
void SubmitScene::Update(float deltaTime) {
    ticks += deltaTime;
    if (ticks > 4 && ticks < 100 &&
        dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"))->MapId == 2) {
        ticks = 100;
        bgmId = AudioHelper::PlayBGM("happy.ogg");
    }
}
void SubmitScene::BackOnClick(int stage) {
    // Change to select scene.
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
void SubmitScene::SubmitOnClick(int stage){
    std::string filename = "../Resource/scoreboard.txt";
    std::ifstream fin(filename);
    std::vector<std::string>files;
    std::string buffer;
    while(getline(fin, buffer))files.push_back(buffer);
    fin.close();
    std::ofstream fout(filename);
    for(auto it:files)fout<<it<<'\n';
    fout<<name + " " + std::to_string(score) + " " + std::to_string(EndTime) + " " + std::to_string(EndTime - StartTime)<<'\n';
    SubmitScene::BackOnClick(1);
}
void SubmitScene::CulGrade(){
    if(MapId == 1){
        score = ((StartTime - EndTime) + 100) + money / 10 + lives*10;
    }else{
        score = 0;
    }
}

void SubmitScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    std::cerr<<keyCode<<" olasdff\n";
    if(keyCode == 63){
        name.pop_back();
    }else if(ALLEGRO_KEY_A <= keyCode && keyCode <= ALLEGRO_KEY_Z){
        name.push_back((char)(keyCode - ALLEGRO_KEY_A + 'A'));
    }
    NameLabel->Text = name;
}