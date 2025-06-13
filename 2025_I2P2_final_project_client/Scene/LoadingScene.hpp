#ifndef LOADINGSCENE_HPP
#define LOADINGSCENE_HPP

#include "Engine/IScene.hpp"
#include "Engine/Group.hpp"
#include "Engine/Sprite.hpp"
#include "UI/Component/Label.hpp"

#include <vector>

class LoadingScene final : public Engine::IScene {
private:
    bool mapSent = false; // Whether map2 has been sent
    float ticks = 0;
    float accumulatedTime;
    int dot_num; float dot_cooldown;
    
public:
    int MapNum=1;
    Engine::Label *TextLoading;
    std::vector<Engine::Sprite*>ImageLoading;
    void Initialize() override;
    void Update(float deltaTime) override;
    void ConstructUI();
    Group *UIGroup;
};

#endif // LOADINGSCENE_HPP