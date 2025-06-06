#ifndef SUBMITSCENE_HPP
#define SUBMITSCENE_HPP
#include "Engine/IScene.hpp"
#include <allegro5/allegro_audio.h>
#include <memory>
#include <vector>
class SubmitScene final : public Engine::IScene {
private:
    float ticks;
    ALLEGRO_SAMPLE_ID bgmId;
    Engine::Label* NameLabel;

public:
    int lives;
    int money;
    int SpeedMult;
    int MapId;
    time_t StartTime;
    time_t EndTime;
    int score;
    std::string name;
    explicit SubmitScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void BackOnClick(int stage);
    void SubmitOnClick(int stage);
    void CulGrade();
    void OnKeyDown(int keyCode) override;
};

#endif   // WINSCENE_HPP