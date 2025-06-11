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
    static const int MapWidth, MapHeight;
    static const int BlockSize;
protected:


public:
    std::unordered_map<int, Player*> player_dict;
    std::unordered_map<int, Engine::Sprite*> Tile_dict;
    std::vector<std::vector<nlohmann::json>>MapState;
    std::vector<std::vector<int>>mapState_2;
    Group *PlayerGroup;
    Group *TileMapGroup;
    Group *UIGroup;

    Engine::Sprite * preview;
    Engine::Image *imgTarget;

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
};

#endif   // PLAYSCENE_HPP
