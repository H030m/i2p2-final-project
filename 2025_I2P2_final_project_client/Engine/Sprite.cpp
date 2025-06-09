#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <memory>
#include <iostream>
#include "Engine/Point.hpp"
#include "Sprite.hpp"
#include "UI/Component/Image.hpp"

#include "Engine/GameEngine.hpp"
int Engine::Sprite::id_counter = 0;
namespace Engine {
    Sprite::Sprite(std::string img, float x, float y, float w, float h, float anchorX, float anchorY,
                   float rotation, float vx, float vy, unsigned char r, unsigned char g, unsigned char b, unsigned char a, int id) : Image(img, x, y, w, h, anchorX, anchorY,id), Rotation(rotation), Velocity(Point(vx, vy)), Tint(al_map_rgba(r, g, b, a)), img_path(img), id(id){
        // id = id_counter++;
        SourceW = GetBitmapWidth();
        SourceH = GetBitmapHeight();
    }
    void Sprite::Draw() const {
        std::cerr << "[SpriteDraw] id=" << id
          << " Pos=(" << Position.x << "," << Position.y << ")"
          << " Size=(" << Size.x << "," << Size.y << ")"
          << " Anchor=(" << Anchor.x << "," << Anchor.y << ")"
          << " SrcWH=(" << SourceW << "," << SourceH << ")"
          << '\n';

        al_draw_tinted_scaled_rotated_bitmap_region(
            bmp.get(), SourceX, SourceY, SourceW, SourceH, Tint,
            Anchor.x * SourceW, Anchor.y * SourceH,
            Position.x, Position.y,
            Size.x / SourceW, Size.y / SourceH,
            Rotation, 0
        );

        al_draw_rectangle(
            Position.x - Size.x / 2,
            Position.y - Size.y / 2,
            Position.x + Size.x / 2,
            Position.y + Size.y / 2,
            al_map_rgb(255, 0, 0), 2
        );

    }
    void Sprite::Update(float deltaTime) {
        Position.x += Velocity.x * deltaTime;
        Position.y += Velocity.y * deltaTime;
    }
};
