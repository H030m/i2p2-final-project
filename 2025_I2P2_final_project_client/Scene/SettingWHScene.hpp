#ifndef SETTINGWHSCENE_HPP
#define SETTINGWHSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

class SettingWHScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    Engine::Label *TextH, *TextW;
    Slider *SliderH, *SliderW;
    int H, W;

public:
    explicit SettingWHScene() = default;
    void Initialize() override;
    void Update(float deltaTime) override;
    void OnKeyDown(int keyCode) override;
    void GenerateMapAndEnter();
    void BackOnClick(int stage);
};

#endif   // STAGESELECTSCENE_HPP
