#ifndef DRAWMAPCSCENE_HPP
#define DRAWMAPCSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>
#include <nlohmann/json.hpp>

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include <unordered_map>
#include "Player/Player.hpp"
#include "Engine/Sprite.hpp"
#include "Camera/Camera.hpp"
#include "UI/Component/ImageButton.hpp"
class Turret;
namespace Engine {
    class Group;
    class Image;
    class Label;
    class Sprite;
}   // namespace Engine

class DrawMapScene final : public Engine::IScene {
private:
    enum TileType {
        TILE_DIRT,
        TILE_FLOOR,
        TILE_OCCUPIED,
    };
    static int MapWidth, MapHeight;
    static const int BlockSize;
    bool pause = false;
    float save_cooldown;
    std::unique_ptr<Camera> camera;
    void RenderVisibleTiles() const;
    void RenderVisibleObjects() const;
    std::string SaveRoute[3] = {"Resource/map1.json", "Resource/map2.json","Resource/map3.json"};
protected:


public:
    std::unordered_map<int, Player*> player_dict;
    std::unordered_map<int, Engine::Sprite*> Tile_dict;
    std::unordered_map<int, Engine::Sprite*> Obstacle_dict;
    std::vector<std::vector<nlohmann::json>>MapState;
    std::vector<std::vector<int>>mapState_2;
    Group *PlayerGroup;
    Group *TileMapGroup;
    Group *UIGroup;
    Group *ObstacleGroup;
    Group *LabelGroup;
    Group *GroundEffectGroup;
    Engine::Sprite * preview;
    Engine::Image *imgTarget;
    Engine::Image *correct = nullptr; float correct_time;
    std::vector<std::pair<Engine::ImageButton*,Engine::Label*>> SelectFile;
    explicit DrawMapScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void OnMouseDown(int button, int mx, int my) override;
    void OnMouseMove(int mx, int my) override;
    void OnMouseUp(int button, int mx, int my) override;
    void OnKeyDown(int keyCode) override;
    void OnKeyUp(int keyCode) override;
    void ReadMap();
    void ConstructUI();
    void UIBtnClicked(int id);
    void SaveMapStateToFile(const std::string& path);

    Camera* GetCamera() const { return camera.get(); }
    int MapNum = 1;
};

#endif   // PLAYSCENE_HPP
