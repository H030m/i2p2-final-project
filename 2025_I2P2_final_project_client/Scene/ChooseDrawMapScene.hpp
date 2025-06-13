#ifndef CHOOSEDRAWMAPSCENE_HPP
#define CHOOSEDRAWMAPSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"

class ChooseDrawMapScene final : public Engine::IScene {
private:


public:
    explicit ChooseDrawMapScene() = default;
    void Initialize() override;
    void Terminate() override;
    void PlayOnClick(int stage);
    void BackOnClick();
};

#endif   // CHOOSEMAP_HPP
