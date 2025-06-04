#ifndef SCOREBOARDSCENE_HPP
#define SCOREBOARDSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>
#include <vector>
#include "Engine/IScene.hpp"



class ScoreboardScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    std::vector<PlayerInformation> scoreboard;
    std::vector<std::vector<Engine::Label*>>label;
public:
    explicit ScoreboardScene() = default;
    void Initialize() override;
    void Terminate() override;
    void BackOnClick(int stage);
    void LoadPage(int k);
};


#endif   // SCOREBOARDSCENE_HPP