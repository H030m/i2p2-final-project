#ifndef WINSCENE_HPP
#define WINSCENE_HPP
#include "Engine/IScene.hpp"
#include <allegro5/allegro_audio.h>

class WinScene final : public Engine::IScene {
private:
    float ticks;
    ALLEGRO_SAMPLE_ID bgmId;



public:

    int lives;
    int money;
    int SpeedMult;
    int MapId;
    time_t StartTime;
    time_t EndTime;
    int score;
    
    explicit WinScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void BackOnClick(int stage);
    void SubmitOnClick(int stage);
    void CulGrade();
};

#endif   // WINSCENE_HPP
