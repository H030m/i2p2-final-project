#ifndef LOADINGSCENE_HPP
#define LOADINGSCENE_HPP

#include "Engine/IScene.hpp"

class LoadingScene final : public Engine::IScene {
private:
    bool mapSent = false; // Whether map2 has been sent
    float ticks = 0;
    float accumulatedTime;
public:
    void Initialize() override;
    void Update(float deltaTime) override;
};

#endif // LOADINGSCENE_HPP