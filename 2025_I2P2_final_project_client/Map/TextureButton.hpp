#ifndef TEXTUREBUTTON
#define TEXTUREBUTTON
#include <string>

#include "Engine/Sprite.hpp"
#include "UI/Component/ImageButton.hpp"

class DrawMapScene;

class TextureButton : public Engine::ImageButton {
protected:
    DrawMapScene *getPlayScene();

public:

    Engine::Sprite TileTexture;
    TextureButton(std::string img, std::string imgIn, Engine::Sprite TileTexture, float x, float y);
    void Update(float deltaTime) override;
    void Draw() const override;
};
#endif   // TURRETBUTTON_HPP 