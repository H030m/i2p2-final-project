#include <algorithm>
#include <allegro5/allegro.h>
#include <cmath>
#include <fstream>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <ctime>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Scene/ScoreboardScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"
int total_page, page;
const int NUM_IN_PAGE = 7;
std::string dirt_png = "Resource/images/stage-select/dirt.png";
std::string floor_png = "Resource/images/stage-select/floor.png";
std::string pirulen_font = "pirulen.ttf";
void ScoreboardScene::Initialize(){
    //input data
    std::string filename = "../Resource/scoreboard.txt";
    std::ifstream fin(filename);
    std::string buffer;
    scoreboard.clear();
    while(getline(fin,buffer)){
        std::stringstream ss(buffer);
        int score, gap;
        std::time_t Time;
        std::string name;
        ss>>name>>score>>Time>>gap;
        scoreboard.push_back(PlayerInformation(name, score, Time, gap));
    }
    fin.close();
    std::sort(scoreboard.begin(),scoreboard.end(),std::greater<PlayerInformation>());

    // page
    page = 0;
    label.clear();
    label.resize(scoreboard.size(), std::vector<Engine::Label*>(3, nullptr));
    total_page = scoreboard.size() / NUM_IN_PAGE + (scoreboard.size() % NUM_IN_PAGE != 0);
    this->LoadPage(page);
    

    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    // back button
    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 + 500, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::BackOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH / 2 + 550, 0, 0, 0, 255, 0.5, 0.5));
    // next page
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 300  + 600, halfH / 2 + 500, 300, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::LoadPage, this,1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Next", "pirulen.ttf", 48, halfW + 450, halfH / 2 + 550, 0, 0, 0, 255, 0.5, 0.5));

    //prev
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 300 - 300, halfH / 2 + 500, 300, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::LoadPage, this, -1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("prev", "pirulen.ttf", 48, halfW - 450, halfH / 2 + 550, 0, 0, 0, 255, 0.5, 0.5));

    // scoreboard Title
    Engine::Label *Title = new Engine::Label("SCOREBOARD", "pirulen.ttf", 110, halfW, 100, 255, 215, 0, 255, 0.5, 0.5);
    AddNewObject(Title);

    // BGM
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);


}
void ScoreboardScene::LoadPage(int k){
    std::cerr<<total_page<<' '<<page<<'\n';
    page += k;
    if(page < 0) page = 0;
    if(page >= total_page)page = total_page - 1;
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    
    int x = halfW - 450, y = 150;
    for(int i = page * NUM_IN_PAGE, j = 0; i < (page + 1) * NUM_IN_PAGE; i++, j++){
        // new
        if(label[j][0] == nullptr&& i < scoreboard.size()){
            label[j][0] = new Engine::Label(scoreboard[i].name, pirulen_font, 64, 100, y + j*75, 100, 255, 100);
            AddNewObject(label[j][0]);
        }
        if(label[j][1] == nullptr&& i < scoreboard.size()){
            label[j][1] = new Engine::Label(std::to_string(scoreboard[i].score), pirulen_font, 64, x+600, y + j*75, 100, 255, 100);
            AddNewObject(label[j][1]);
        }
        if(label[j][2] == nullptr&& i < scoreboard.size()){
            label[j][2] = new Engine::Label(scoreboard[i].TimeToString_Time(), pirulen_font, 12, x+900, y + j*75 + 40, 100, 255, 100);
            AddNewObject(label[j][2]);
        }     
        
        // update
        label[j][0]->Text = (i < scoreboard.size())?scoreboard[i].name:"";
        label[j][1]->Text = (i < scoreboard.size())?std::to_string(scoreboard[i].score):"";
        label[j][2]->Text = (i < scoreboard.size())?scoreboard[i].TimeToString_Time():"";
        
        //Label(const std::string &text, const std::string &font, int fontSize, float x, float y, unsigned char r = 0, unsigned char g = 0, unsigned char b = 0, unsigned char a = 255, float anchorX = 0, float anchorY = 0);
    }
}
void ScoreboardScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
void ScoreboardScene::Terminate() {
    
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}