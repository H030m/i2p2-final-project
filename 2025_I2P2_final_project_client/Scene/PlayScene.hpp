#ifndef PLAYSCENE_HPP
#define PLAYSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>
#include <nlohmann/json.hpp>
#include <string>
#include <set>
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include <unordered_map>
#include "Player/Player.hpp"
#include "Camera/Camera.hpp"
#include "UI/Component/ImageButton.hpp"
class Weapon;
namespace Engine {
    class Group;
    class Image;
    class Label;
    class Sprite;
}   // namespace Engine

class PlayScene final : public Engine::IScene {
private:
    enum TileType {
        TILE_DIRT,
        TILE_FLOOR,
        TILE_OCCUPIED,
    };
    ALLEGRO_SAMPLE_ID bgmId;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> deathBGMInstance;\


    std::unique_ptr<Camera> camera;
    void RenderVisibleTiles() const;
    void RenderVisibleObjects() const;
    void ClearPlayerData();
    void ClearEnemyData();

protected:
    int lives;
    int money;
    int SpeedMult;
    
    time_t StartTime;
    time_t EndTime;

public:
    std::unordered_map<int, Player*> player_dict;
    std::unordered_map<int, Enemy*> enemy_dict;
    std::unordered_map<int, std::vector<Engine::Label*>> player_UI_Label;
    std::unordered_map<int, std::vector<Engine::ImageButton*>> player_UI_Button;
    static bool DebugMode;
    static const std::vector<Engine::Point> directions;
    static int MapWidth, MapHeight;
    static const int BlockSize;
    static const float DangerTime;
    static const Engine::Point SpawnGridPoint;
    static const std::vector<int> code;
    int MapId;
    float ticks;
    float deathCountDown;
    // Map tiles.
    Group *TileMapGroup;
    Group *GroundEffectGroup;
    Group *DebugIndicatorGroup;
    Group *BulletGroup;
    Group *EnemyGroup;
    Group *EffectGroup;
    Group *UIGroup;
    Group *PlayerGroup;
    Group *WeaponGroup;
    Group *ObstacleGroup;
    Engine::Label *UIMoney;
    Engine::Label *UILives;
    Engine::Image *imgTarget;
    Engine::Sprite *dangerIndicator;
    
    // Turret *preview;
    std::vector<std::vector<nlohmann::json>> mapState;
    std::vector<std::vector<int>> mapDistance;
    std::list<std::pair<int, float>> enemyWaveData;
    std::list<int> keyStrokes;

    int state = 0;

    static Engine::Point GetClientSize();
    explicit PlayScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void OnMouseDown(int button, int mx, int my) override;
    void OnMouseMove(int mx, int my) override;
    void OnMouseUp(int button, int mx, int my) override;
    void OnKeyDown(int keyCode) override;
    void OnKeyUp(int keyCode) override;
    void Hit();
    int GetMoney() const;
    void EarnMoney(int money);
    void ReadMap();
    void ReadEnemyWave();
    void ConstructUI();
    void UIBtnClicked(int id);
    bool CheckSpaceValid(int x, int y);

    //Given the coordinates, return whether it can be moved
    bool isWalkable(int x, int y, int radius);
    std::vector<std::vector<int>> CalculateBFSDistance();
    


    int id_counter;
    int my_id;
    Engine::Label *upgrade_label_1, *upgrade_label_2;
    Camera* GetCamera() const { return camera.get(); }
    std::set<int>PlayerWeapon;
};

#endif   // PLAYSCENE_HPP
