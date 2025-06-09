#include <allegro5/allegro.h>
#include <memory>

#include "Engine/Point.hpp"
#include "Sprite.hpp"
#include "UI/Component/Image.hpp"

#include "Engine/GameEngine.hpp"
#include "Connect/RenderSender.hpp"
int Engine::Sprite::id_counter = 0;
namespace Engine {
    Sprite::Sprite(std::string img, float x, float y, float w, float h, float anchorX, float anchorY,
                   float rotation, float vx, float vy, unsigned char r, unsigned char g, unsigned char b, unsigned char a) : Image(img, x, y, w, h, anchorX, anchorY), Rotation(rotation), Velocity(Point(vx, vy)), Tint(al_map_rgba(r, g, b, a)), img_path(img){
        id = id_counter++;
        SourceW = GetBitmapWidth();
        SourceH = GetBitmapHeight();
    }
    void Sprite::Draw() const {
        al_draw_tinted_scaled_rotated_bitmap_region(bmp.get(), SourceX, SourceY, SourceW, SourceH, Tint, 
            Anchor.x * SourceW, Anchor.y * SourceH, // anchor in cropped region
            Position.x, Position.y,
            Size.x / SourceW, Size.y / SourceH,
            Rotation, 0
        );
        
        
        nlohmann::json spriteJson = {
        {"type", "sprite"},
        {"img", img_path},
        {"x", Position.x},
        {"y", Position.y},
        {"w", Size.x},
        {"h", Size.y},
        {"rotation", Rotation},
        {"r", int(Tint.r * 255)},
        {"g", int(Tint.g * 255)},
        {"b", int(Tint.b * 255)},
        {"a", int(Tint.a * 255)},
        {"id", id},
        {"SX", SourceX},
        {"SY", SourceY},
        {"SH", SourceH},
        {"SW", SourceW},
        };
        RenderSender& sender = Engine::GameEngine::GetInstance().GetSender();
        sender.AddToFrame(spriteJson);
    }
    void Sprite::Update(float deltaTime) {
        Position.x += Velocity.x * deltaTime;
        Position.y += Velocity.y * deltaTime;
    }
};
