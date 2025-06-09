#include "IObject.hpp"
#include "Engine/Point.hpp"

namespace Engine {
    
    IObject::IObject(float x, float y, float w, float h, float anchorX, float anchorY, int id) : Position(Point(x, y)), Size(Point(w, h)), Anchor(Point(anchorX, anchorY)), type_id(id) {}
    std::list<std::pair<bool, IObject *>>::iterator IObject::GetObjectIterator() const {
        return objectIterator;
    }
    void IObject::Draw() const {}
    void IObject::Update(float deltaTime) {}
}
