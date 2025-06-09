#include <allegro5/allegro.h>
#include <memory>

#include "Engine/IObject.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Engine/GameEngine.hpp"
#include "Image.hpp"

#include "Connect/RenderSender.hpp"
int Engine::Image::id_counter = 0;
namespace Engine {
    Image::Image(std::string img, float x, float y, float w, float h, float anchorX, float anchorY) : IObject(x, y, w, h, anchorX, anchorY), img_path(img) {
        if (Size.x == 0 && Size.y == 0) {
            bmp = Resources::GetInstance().GetBitmap(img);
            Size.x = GetBitmapWidth();
            Size.y = GetBitmapHeight();
        } else if (Size.x == 0) {
            bmp = Resources::GetInstance().GetBitmap(img);
            Size.x = GetBitmapWidth() * Size.y / GetBitmapHeight();
        } else if (Size.y == 0) {
            bmp = Resources::GetInstance().GetBitmap(img);
            Size.y = GetBitmapHeight() * Size.x / GetBitmapWidth();
        } else /* Size.x != 0 && Size.y != 0 */ {
            bmp = Resources::GetInstance().GetBitmap(img, Size.x, Size.y);
        }
        id = id_counter++;
    }
    void Image::Draw() const {
        al_draw_scaled_bitmap(bmp.get(), 0, 0, GetBitmapWidth(), GetBitmapHeight(),
                              Position.x - Anchor.x * GetBitmapWidth(), Position.y - Anchor.y * GetBitmapHeight(),
                              Size.x, Size.y, 0);
        RenderSender& sender = Engine::GameEngine::GetInstance().GetSender();
        nlohmann::json imageJson = {
            {"type", "image"}, 
            {"img", img_path},  
            {"x", Position.x},
            {"y", Position.y},
            {"w", Size.x},
            {"h", Size.y},
            {"id",id},
            {"AnchorX",Anchor.x},
            {"AnchorY",Anchor.y}
        };
        sender.AddToFrame(imageJson);
    }
    int Image::GetBitmapWidth() const {
        return al_get_bitmap_width(bmp.get());
    }
    int Image::GetBitmapHeight() const {
        return al_get_bitmap_height(bmp.get());
    }
}
