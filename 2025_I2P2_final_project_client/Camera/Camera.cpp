// Camera.cpp
#include "Camera.hpp"
#include <algorithm>
#include <cmath>

Camera::Camera(float viewportWidth, float viewportHeight)
    : viewportSize(viewportWidth, viewportHeight) {
    position = Engine::Point(0, 0);
    targetPosition = Engine::Point(0, 0);
}

void Camera::SetTarget(const Engine::Point& target) {
    targetPosition = target;
}

void Camera::Update(float deltaTime) {
    position.x = targetPosition.x;
    position.y = targetPosition.y;
    
}

Engine::Point Camera::WorldToScreen(const Engine::Point& worldPos) const {
    return Engine::Point(
        worldPos.x - position.x + viewportSize.x / 2,
        worldPos.y - position.y + viewportSize.y / 2
    );
}

Engine::Point Camera::ScreenToWorld(const Engine::Point& screenPos) const {
    return Engine::Point(
        screenPos.x + position.x - viewportSize.x / 2,
        screenPos.y + position.y - viewportSize.y / 2
    );
}

bool Camera::IsInView(const Engine::Point& worldPos, float objectRadius) const {
    float halfWidth = viewportSize.x / 2 + objectRadius;
    float halfHeight = viewportSize.y / 2 + objectRadius;
    
    return (worldPos.x >= position.x - halfWidth &&
            worldPos.x <= position.x + halfWidth &&
            worldPos.y >= position.y - halfHeight &&
            worldPos.y <= position.y + halfHeight);
}

Camera::VisibleArea Camera::GetVisibleTileArea(int tileSize) const {
    float halfWidth = viewportSize.x / 2;
    float halfHeight = viewportSize.y / 2;
    
    // 計算可見的瓦片範圍，並添加一些邊界
    int buffer = 0; // 額外渲染的瓦片數量
    
    VisibleArea area;
    area.minX = std::max(0, (int)((position.x - halfWidth) / tileSize) - buffer);
    area.maxX = (int)((position.x + halfWidth) / tileSize) + buffer;
    area.minY = std::max(0, (int)((position.y - halfHeight) / tileSize) - buffer);
    area.maxY = (int)((position.y + halfHeight) / tileSize) + buffer;

    return area;
}