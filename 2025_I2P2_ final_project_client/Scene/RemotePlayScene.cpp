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

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Resources.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/Plane.hpp"
#include "UI/Component/Label.hpp"
#include "RemotePlayScene.hpp"

void RemotePlayScene::Initialize(){
    AddNewObject(AllSprite = new Group());
    AddNewObject(AllImage = new Group());
    AddNewObject(AllLabel = new Group());
}

void RemotePlayScene::Terminate() {

    IScene::Terminate();
}
void RemotePlayScene::Update(float deltaTime) {
    Engine::GameEngine &game = Engine::GameEngine::GetInstance();
    GameClient &sender = game.GetSender();
    int x = 0,y = 0;
    if(sender.input_json.contains("x")){
        x = sender.input_json["x"], y = sender.input_json["y"];
    }
    // first deal Image
    // deafualt all should be erase
    auto Objects = AllImage->GetObjects();
    // std::cerr<<"this\n";
    int tmp = 1; 
    for(auto it: Objects){
        auto iter = it->GetObjectIterator();
        iter->first = false;
    }

    
    // std::cerr<<"hello1"<<'\n';
    for(auto it:sender.input_json["image"]){
        if(IdToImage.count(it["id"])){
            std::cerr<<"shit"<<it["id"]<<"\n";
            Engine::Image* img = dynamic_cast<Engine::Image*>(IdToImage[it["id"]]->second);;
            img->Position.x = (int)it["x"] - x;
            img->Position.y = (int)it["y"] - y;
            img->Size.x = it["w"];
            img->Size.y = it["h"];
            IdToImage[it["id"]]->first = true;
        }else{
            auto* image = new Engine::Image(it["img"], it["x"], it["y"], it["w"], it["h"], it["AnchorX"], it["AnchorY"], it["id"]);
            AllSprite->AddNewObject(image);
            IdToSprite[it["id"]] = image->GetObjectIterator();
                                              
        }
        std::cerr<<"Image id:"<<it["id"]<<'\n';
    }
    for (auto it = IdToImage.begin(); it != IdToImage.end(); ) {
        if (it->first == 0) {
            it = IdToImage.erase(it); 
        } else {
            ++it;
        }
    }
    Objects = AllImage->GetObjects();
    for(auto it:Objects){
        AllImage->RemoveObject(it->GetObjectIterator());
    }
    
    // std::cerr<<"hoho\n";
    // Sprite
    // auto SpriteObjects = AllSprite->GetObjects();
    // for (auto it : SpriteObjects) {
    //     auto iter = it->GetObjectIterator();
    //     iter->first = false;
    // }

    // for (auto it : sender.input_json["sprite"]) {
    //     int id = it["id"];
    //     if (IdToSprite.count(id)) {
    //         Engine::Sprite* spr = dynamic_cast<Engine::Sprite*>(IdToSprite[id]->second);
    //         if (spr) {
    //             spr->Position.x = it["x"];
    //             spr->Position.y = it["y"];
    //             spr->Size.x = it["w"];
    //             spr->Size.y = it["h"];
    //             spr->Rotation = it["rotation"];
    //             spr->Tint.r = it["r"];
    //             spr->Tint.g = it["g"];
    //             spr->Tint.b = it["b"];
    //             spr->Tint.a = it["a"];
    //             IdToSprite[id]->first = true;
    //         }
    //     } else {
    //         AllSprite->AddNewObject(new Engine::Sprite(
    //             it["img"], it["x"], it["y"], it["w"], it["h"],
    //             it["AnchorX"], it["AnchorY"], it["rotation"], 0, 0,
    //             it["r"], it["g"], it["b"], it["a"], id
    //         ));

    //         SpriteObjects = AllSprite->GetObjects();
    //         for (auto spr : SpriteObjects) {
    //             if (spr->type_id == id) {
    //                 IdToSprite[id] = spr->GetObjectIterator();
    //             }
    //         }
    //     }
    //     std::cerr<<"Sprite id:"<<it["id"]<<'\n';
    // }

    // for (auto it = IdToSprite.begin(); it != IdToSprite.end(); ) {
    //     if (it->second->first == false) {
    //         it = IdToSprite.erase(it);
    //     } else {
    //         ++it;
    //     }
    // }

    // AllSprite->RemoveObject(AllSprite->GetObjectIterator());
}
void RemotePlayScene::Draw() const{
    IScene::Draw();
}
