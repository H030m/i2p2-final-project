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



}

void RemotePlayScene::Terminate() {
    // AudioHelper::StopBGM(bgmId);
    // AudioHelper::StopSample(deathBGMInstance);
    // deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void RemotePlayScene::Update(float deltaTime) {

}
void RemotePlayScene::Draw() const{
    IScene::Draw();
}
