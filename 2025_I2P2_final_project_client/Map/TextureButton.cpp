#include <allegro5/color.h>

#include "Engine/GameEngine.hpp"
#include "Engine/IScene.hpp"
#include "Scene/DrawMapScene.hpp"
#include "TextureButton.hpp"

DrawMapScene *TextureButton::getPlayScene() {
    return dynamic_cast<DrawMapScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}
TextureButton::TextureButton(std::string img, std::string imgIn, Engine::Sprite TileTexture, float x, float y) : ImageButton(img, imgIn, x, y), TileTexture(TileTexture) {
}
void TextureButton::Update(float deltaTime) {
    ImageButton::Update(deltaTime);
}
void TextureButton::Draw() const {
    ImageButton::Draw();
    TileTexture.Draw();
}