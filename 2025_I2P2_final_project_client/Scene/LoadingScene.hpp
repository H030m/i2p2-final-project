#ifndef LOADINGSCENE_HPP
#define LOADINGSCENE_HPP

#include "Engine/IScene.hpp"
#include "Engine/Group.hpp"
#include "Engine/Sprite.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/ImageButton.hpp"
#include <vector>

class LoadingScene final : public Engine::IScene {
private:
    bool mapSent = false; // Whether map2 has been sent
    float ticks = 0;
    float accumulatedTime;
    int dot_num; float dot_cooldown;
    std::vector<bool>weaponSelected;
    std::vector<Engine::ImageButton*>WeaponButtons;
    std::vector<Engine::Image*> WeaponBorders;
    int selectedCount = 0;
    bool mapReceived = false;
public:
    int MapNum=1;
    Engine::Label *TextLoading;
    std::vector<Engine::Sprite*>ImageLoading;
    Engine::ImageButton* ConfirmButton;
    void Initialize() override;
    void Update(float deltaTime) override;
    void ConstructUI();
    void OnClickWeapon(int type);
    void OnClickConfirm();
    Group *UIGroup;
};

#endif // LOADINGSCENE_HPP