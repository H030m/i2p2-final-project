// Camera.cpp
#include "Camera.hpp"
#include <algorithm>
#include <cmath>
<<<<<<< HEAD
#include <iostream>

Camera::Camera(float viewportWidth, float viewportHeight, Engine::Point margin)
    : viewportSize(viewportWidth, viewportHeight), margin(margin) {
    position = Engine::Point(0, 0);
    targetPosition = Engine::Point(0, 0);

=======

Camera::Camera(float viewportWidth, float viewportHeight)
    : viewportSize(viewportWidth, viewportHeight) {
    position = Engine::Point(0, 0);
    targetPosition = Engine::Point(0, 0);
>>>>>>> 99b35bcf769e3dedb1dc8ef8dc6a0985c2d7badc
}

void Camera::SetTarget(const Engine::Point& target) {
    targetPosition = target;
}

void Camera::Update(float deltaTime) {
<<<<<<< HEAD

    float halfW = viewportSize.x / 2;
    float halfH = viewportSize.y / 2;
   
    // ¥­·Æ²¾°Ê camera.position ¡÷ targetPosition
    float speed = 5.0f;
    position.x += (targetPosition.x - position.x) * speed * deltaTime;
    position.y += (targetPosition.y - position.y) * speed * deltaTime;

    // Clamp ³Ì²× position ¤£­n¶W¥XÃä¬É¡]¥H margin ¬°¦a¹ÏÃä¬É¡^
    float minX = halfW;
    float maxX = margin.x - halfW;
    float minY = halfH;
    float maxY = margin.y - halfH;

    position.x = std::max(minX, std::min(position.x,maxX));
    position.y = std::max(minY, std::min(position.y,maxY));
    std::cerr<<"target"<<' '<<targetPosition.x<<' '<<targetPosition.y<<' '<<"position "<<position.x<<' '<<position.y<<" margin "<<margin.x<<' '<<margin.y<<' '<<minX<<' '<<maxX<<' '<<minY<<' '<<maxY<<'\n';
=======
    position.x = targetPosition.x;
    position.y = targetPosition.y;
    
>>>>>>> 99b35bcf769e3dedb1dc8ef8dc6a0985c2d7badc
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
    
<<<<<<< HEAD
    // ­pºâ¥i¨£ªº¥Ë¤ù½d³ò¡A¨Ã²K¥[¤@¨ÇÃä¬É
    int buffer = 0; // ÃB¥~´è¬Vªº¥Ë¤ù¼Æ¶q
=======
    // è¨ˆç®—å¯è¦‹çš„ç“¦ç‰‡ç¯„åœï¼Œä¸¦æ·»åŠ ä¸€äº›é‚Šç•Œ
    int buffer = 0; // é¡å¤–æ¸²æŸ“çš„ç“¦ç‰‡æ•¸é‡
>>>>>>> 99b35bcf769e3dedb1dc8ef8dc6a0985c2d7badc
    
    VisibleArea area;
    area.minX = std::max(0, (int)((position.x - halfWidth) / tileSize) - buffer);
    area.maxX = (int)((position.x + halfWidth) / tileSize) + buffer;
    area.minY = std::max(0, (int)((position.y - halfHeight) / tileSize) - buffer);
    area.maxY = (int)((position.y + halfHeight) / tileSize) + buffer;

    return area;
}