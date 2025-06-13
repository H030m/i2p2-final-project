#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"
#include "ChooseDrawMapScene.hpp"
#include "Scene/LoadingScene.hpp"
#include "DrawMapScene.hpp"
void ChooseDrawMapScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    //Map 1
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 300, halfH / 2 - 100, 600, 100);
    btn->SetOnClickCallback(std::bind(&ChooseDrawMapScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Map1", "pirulen.ttf", 48, halfW, halfH / 2 -50, 0, 0, 0, 255, 0.5, 0.5));
    
    //Map2
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 300, halfH / 2 + 50, 600, 100);
    btn->SetOnClickCallback(std::bind(&ChooseDrawMapScene::PlayOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Map2", "pirulen.ttf", 48, halfW, halfH / 2 + 100, 0, 0, 0, 255, 0.5, 0.5));
    
    //Map3
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 300, halfH / 2 + 200, 600, 100);
    btn->SetOnClickCallback(std::bind(&ChooseDrawMapScene::PlayOnClick, this,3));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Map3", "pirulen.ttf", 48, halfW, halfH / 2 + 250, 0, 0, 0, 255, 0.5, 0.5));
    
    //Map4
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 300, halfH / 2 + 350, 600, 100);
    btn->SetOnClickCallback(std::bind(&ChooseDrawMapScene::PlayOnClick, this,5));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Deafualt", "pirulen.ttf", 48, halfW, halfH / 2 + 400, 0, 0, 0, 255, 0.5, 0.5));

    //TODO back
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 300, halfH / 2 + 500, 600, 100);
    btn->SetOnClickCallback(std::bind(&ChooseDrawMapScene::BackOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("BACK", "pirulen.ttf", 48, halfW, halfH / 2 + 550, 0, 0, 0, 255, 0.5, 0.5));

    
}

void ChooseDrawMapScene::Terminate() {

}
void ChooseDrawMapScene::PlayOnClick(int stage){
    DrawMapScene *scene = dynamic_cast<DrawMapScene *>(Engine::GameEngine::GetInstance().GetScene("DrawMapScene"));
    std::cerr<<scene->MapNum<<"bug1\n";
    scene->MapNum = stage;
    if(stage == 5){
        Engine::GameEngine::GetInstance().ChangeScene("SettingWHScene");
        return;
    }
    Engine::GameEngine::GetInstance().ChangeScene("DrawMapScene");
}
void ChooseDrawMapScene::BackOnClick(){
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}