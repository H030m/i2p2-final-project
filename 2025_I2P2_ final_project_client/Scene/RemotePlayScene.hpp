#ifndef REMOTEPLAYSCENE_HPP
#define REMOTEPLAYSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <memory>
#include <map>
#include <list>
#include <utility>

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"

namespace Engine {
    class Group;
    class Image;
    class Label;
    class Sprite;
}   // namespace Engine

class RemotePlayScene final : public Engine::IScene {
private:
    /*
    Sprite
    Label
    Image
    circle
    filledcircle
    */
    Engine::Group AllSprite;
    Engine::Group AllImage;
    std::list<Engine::Label> AllLabel;

    // ? For circle rendering
    struct Circle {
        float x, y;
        float radius;
        float r, g, b;
        float thickness;
    };
    struct FilledCircle {
        float x, y;
        float radius;
        float r, g, b, a;
    };

    std::vector<Circle> AllCircle;
    std::vector<FilledCircle> AllFilledCircle;


public:
    explicit RemotePlayScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;

    
};

#endif