#include <functional>
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "PlayScene.hpp"
#include "Engine/Resources.hpp"
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

    Engine::GameEngine &game = Engine::GameEngine::GetInstance();


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
    // AddNewObject(new Engine::Label("SCORE: " + std::to_string(game.DYYscore/10) + "." + std::to_string(game.DYYscore%10), "pirulen.ttf", 64, halfW, 170, 255, 215, 0, 255, 0.5, 0.5));

    // input your name
    AddNewObject(new Engine::Label("High School road?", "pirulen.ttf", 64, halfW, 350, 255, 255, 255, 255, 0.5, 0.5));
    NameLabel = new Engine::Label("", "pirulen.ttf", 40, halfW, 450, 255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(NameLabel);

    // Preload Lose Scene
    deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
    Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
    // Start BGM.
    bgmId = AudioHelper::PlayBGM("play.ogg");
}
void SubmitScene::Terminate() {
    AudioHelper::StopBGM(bgmId);
    if (deathBGMInstance) {
        AudioHelper::StopSample(deathBGMInstance);
        deathBGMInstance.reset();
    }

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
Engine::GameEngine::GetInstance().ChangeScene("lose");
}
void SubmitScene::SubmitOnClick(int stage){
    if(name == "NANHAI")BackOnClick(1);
    else{
        Engine::GameEngine::GetInstance().ChangeScene("win");
    }
    
}
void SubmitScene::CulGrade(){
    if(MapId == 1){
        score =  money / 10 + lives*10;
    }else{
        score = 0;
    }
}

void SubmitScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    if(keyCode == 63){
        name.pop_back();
    }else if(ALLEGRO_KEY_A <= keyCode && keyCode <= ALLEGRO_KEY_Z){
        name.push_back((char)(keyCode - ALLEGRO_KEY_A + 'A'));
    }
    NameLabel->Text = name;
}