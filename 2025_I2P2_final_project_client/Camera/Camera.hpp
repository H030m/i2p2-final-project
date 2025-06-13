// Camera.hpp
#pragma once
#include "Engine/Point.hpp"

class Camera {
private:
    Engine::Point viewportSize;
    Engine::Point targetPosition;
    Engine::Point margin;
    float smoothing;
    
public:
    Engine::Point position;
    Camera(float viewportWidth, float viewportHeight, Engine::Point margin);
    
    
    void SetTarget(const Engine::Point& target);
    
    
    void Update(float deltaTime);
    
   
    Engine::Point GetPosition() const { return position; }
    
    
    Engine::Point GetViewportSize() const { return viewportSize; }
    
    Engine::Point WorldToScreen(const Engine::Point& worldPos) const;
    
    Engine::Point ScreenToWorld(const Engine::Point& screenPos) const;
    
    bool IsInView(const Engine::Point& worldPos, float objectRadius = 0) const;
    
    
    struct VisibleArea {
        int minX, maxX, minY, maxY;
    };
    VisibleArea GetVisibleTileArea(int tileSize) const;
};