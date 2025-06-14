#include <functional>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "PlayScene.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "WinScene.hpp"

void WinScene::Initialize() {
    // grade
    CulGrade();
    // UI
    ticks = 0;
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    int halfW = w / 2;
    int halfH = h / 2;
    AddNewObject(new Engine::Image("win/SR.png", halfW, halfH, 0, 0, 0.5, 0.5));
    // AddNewObject(new Engine::Label("You Win!", "pirulen.ttf", 48, halfW, halfH / 4 - 10, 255, 255, 255, 255, 0.5, 0.5));
    
    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW -200, halfH * 7 / 4 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&WinScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));
    bgmId = AudioHelper::PlayAudio("win.wav");
    
    // //switch to submit Scene
    // btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW + 50, halfH * 7 / 4 - 50, 400, 100);
    // btn->SetOnClickCallback(std::bind(&WinScene::SubmitOnClick, this, 2));
    // AddNewControlObject(btn);
    // AddNewObject(new Engine::Label("Submit", "pirulen.ttf", 48, halfW + 250, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));
    // bgmId = AudioHelper::PlayAudio("win.wav");
    
    // Grade

    AddNewObject(new Engine::Label("i2p A+!", "pirulen.ttf", 110, halfW, 70, 255, 215, 0, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("YOUR Calc SCORE: " + std::to_string(game.DYYscore/10) + "." + std::to_string(game.DYYscore%10), "pirulen.ttf", 64, halfW, 170, 255, 215, 0, 255, 0.5, 0.5));
}
void WinScene::Terminate() {
    IScene::Terminate();
    AudioHelper::StopBGM(bgmId);
}
void WinScene::Update(float deltaTime) {
    ticks += deltaTime;
    if (ticks > 4 && ticks < 100 &&
        dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"))->MapId == 2) {
        ticks = 100;
        bgmId = AudioHelper::PlayBGM("happy.ogg");
    }
}
void WinScene::BackOnClick(int stage) {
    // Change to select scene.
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
void WinScene::SubmitOnClick(int stage) {
    // Change to select scene.
    Engine::GameEngine::GetInstance().ChangeScene("submit-scene");
}
void WinScene::CulGrade(){
    if(MapId == 1){
        score = money / 10 + lives*10;
    }else{
        score = 0;
    }
}