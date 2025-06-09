#include <allegro5/allegro_font.h>
#include <memory>

#include "Engine/IObject.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Engine/GameEngine.hpp"
#include "Label.hpp"

#include "Connect/RenderSender.hpp"
int Engine::Label::id_counter = 0;
namespace Engine {
    Label::Label(const std::string &text, const std::string &font, int fontSize, float x, float y, unsigned char r, unsigned char g, unsigned char b, unsigned char a, float anchorX, float anchorY) : IObject(x, y, 0, 0, anchorX, anchorY), font(Resources::GetInstance().GetFont(font, fontSize)), Text(text), Color(al_map_rgba(r, g, b, a)), font_path(font) {
        id = id_counter++;
    }
    void Label::Draw() const {
        al_draw_text(font.get(), Color, Position.x - Anchor.x * GetTextWidth(), Position.y - Anchor.y * GetTextHeight(), 0, Text.c_str());
        
        float r, g, b, a;
    al_unmap_rgba_f(Color, &r, &g, &b, &a);

    nlohmann::json textJson = {
        {"type", "text"},
        {"id", id},
        {"text_path", font_path},
        {"x", Position.x - Anchor.x * GetTextWidth()},
        {"y", Position.y - Anchor.y * GetTextHeight()},
        {"flag", 0},
        {"text", Text},
        {"r", int(r * 255)},
        {"g", int(g * 255)},
        {"b", int(b * 255)},
        {"a", int(a * 255)},
    };

    RenderSender& sender = Engine::GameEngine::GetInstance().GetSender();
    sender.AddToFrame(textJson);
    
    }
    int Label::GetTextWidth() const {
        return al_get_text_width(font.get(), Text.c_str());
    }
    int Label::GetTextHeight() const {
        return al_get_font_line_height(font.get());
    }
}
