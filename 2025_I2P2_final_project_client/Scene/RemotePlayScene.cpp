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
using json = nlohmann::json;



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
    // auto Objects = AllImage->GetObjects();
    // // std::cerr<<"this\n";
    // for(auto it: Objects){
    //     auto iter = it->GetObjectIterator();
    //     iter->first = false;
    // }

    
    // // std::cerr<<"hello1"<<'\n';
    // for(auto it:sender.input_json["image"]){
    //     if (it.is_null() || !it.contains("id")) {
    //         std::cerr << "[WARN] Invalid image entry skipped: " << it.dump() << "\n";
    //         continue;
    //     }
       
    //     if(IdToImage.count(it["id"])){
    //         Engine::Image* img = dynamic_cast<Engine::Image*>(IdToImage[it["id"]]->second); 
    //         img->Position.x = (int)it["x"] - x;
    //         img->Position.y = (int)it["y"] - y;
    //         img->Size.x = it["w"];
    //         img->Size.y = it["h"];
    //         IdToImage[it["id"]]->first = true;
    //     }else{
    //         auto* image = new Engine::Image(it["img"], it["x"], it["y"], it["w"], it["h"], it["AnchorX"], it["AnchorY"], it["id"]);
    //         AllImage->AddNewObject(image);
    //         IdToImage[it["id"]] = image->GetObjectIterator();
    //     }
    // }
    // for (auto it = IdToImage.begin(); it != IdToImage.end(); ) {
    // if (!it->second->first) {
    //     AllImage->RemoveObject(it->second); // remove the object from screen
    //     it = IdToImage.erase(it);           // remove from map
    // } else {
    //     ++it;
    // }
    // }
    

    auto SpriteObjects = AllSprite->GetObjects();
    for (auto it : SpriteObjects) {
        auto iter = it->GetObjectIterator();
        iter->first = false;
    }
  
    for (auto it : sender.input_json["sprite"]) {
        if (it.is_null() || !it.contains("id")) {
            std::cerr << "[WARN] Invalid image entry skipped: " << it.dump() << "\n";
            continue;
        }
        // std::cerr << "input sprite id: " << it["id"] << '\n';
        std::vector<std::string> fields = {"x","y","w","h","rotation","SH","SW","SX","SY","r","g","b","a","img"};
        for (const auto& field : fields) {
            if (!it.contains(field) || it[field].is_null()) {
                std::cerr << "[ERROR] Field '" << field << "' in sprite id=" << it["id"] << " is null or missing.\n";
            }
            // std::cerr<<"[hello] "<<field<<' '<<it[field]<<"\n";
        }
        int id = it["id"];
        if (IdToSprite.count(id)) {
            Engine::Sprite* spr = dynamic_cast<Engine::Sprite*>(IdToSprite[id]->second);
            spr->Position.x = it.value("x", 0.0f) - x;
            spr->Position.y = it.value("y", 0.0f) - y;
            spr->Size.x = it.value("w", 0.0f);
            spr->Size.y = it.value("h", 0.0f);
            spr->Rotation = it.value("rotation", 0.0f);
            spr->SourceH = it.value("SH", 64.0f);
            spr->SourceW = it.value("SW", 64.0f);
            spr->SourceX = it.value("SX", 0.0f);
            spr->SourceY = it.value("SY", 0.0f);
            spr->Tint = al_map_rgba(
                it.value("r", 255),
                it.value("g", 255),
                it.value("b", 255),
                it.value("a", 255)
            );
            IdToSprite[id]->first = true;
        } else {
            auto* sprite = new Engine::Sprite(
                it.value("img",""), it.value("x", 0.0f), it.value("y", 0.0f),
                it.value("w", 0.0f), it.value("h", 0.0f),
                it.value("AnchorX", 0.0f),it.value("AnchorY", 0.0f),
                it.value("rotation", 0.0f),
                0,0,
                it.value("r", 255),
                it.value("g", 255),
                it.value("b", 255),
                it.value("a", 255),it["id"]
            );
            sprite->SourceH = it.value("SH", 64.0f);
            sprite->SourceW = it.value("SW", 64.0f);
            sprite->SourceX = it.value("SX", 0.0f);
            sprite->SourceY = it.value("SY", 0.0f);
            if (!sprite->bmp.get()) {
                std::cerr << "[ERROR] Bitmap not loaded: " << it.value("img", "??") << "\n";
            }
            AllSprite->AddNewObject(sprite);
            IdToSprite[id] = sprite->GetObjectIterator();
        }
       
    }

    for (auto it = IdToSprite.begin(); it != IdToSprite.end(); ) {
        if (!it->second->first) {
            AllSprite->RemoveObject(it->second);
            it = IdToSprite.erase(it);
        } else {
            ++it;
        }
    }
    std::cerr << "[DEBUG] AllSprite size: " << AllSprite->GetObjects().size() << "\n";
}
void RemotePlayScene::Draw() const{
    IScene::Draw();
}
