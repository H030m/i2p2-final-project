#ifndef LOADINGSCENE_HPP
#define LOADINGSCENE_HPP

#include "Engine/IScene.hpp"
#include "Engine/Group.hpp"

class LoadingScene final : public Engine::IScene {
private:
    bool mapSent = false; // Whether map2 has been sent
    float ticks = 0;
    float accumulatedTime;
public:
    void Initialize() override;
    void Update(float deltaTime) override;
    Group *UIGroup;
};

#endif // LOADINGSCENE_HPP