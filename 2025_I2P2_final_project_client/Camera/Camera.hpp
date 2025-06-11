// Camera.hpp
#pragma once
#include "Engine/Point.hpp"

class Camera {
private:
    Engine::Point position;
    Engine::Point viewportSize;
    Engine::Point targetPosition;
    Engine::Point margin;
    float smoothing;
    
public:
    Camera(float viewportWidth, float viewportHeight, Engine::Point margin);
    
    // 設置攝像機目標位置（通常是角色位置）
    void SetTarget(const Engine::Point& target);
    
    // 更新攝像機位置（平滑跟隨）
    void Update(float deltaTime);
    
    // 獲取攝像機位置
    Engine::Point GetPosition() const { return position; }
    
    // 獲取視口大小
    Engine::Point GetViewportSize() const { return viewportSize; }
    
    // 世界座標轉換為螢幕座標
    Engine::Point WorldToScreen(const Engine::Point& worldPos) const;
    
    // 螢幕座標轉換為世界座標
    Engine::Point ScreenToWorld(const Engine::Point& screenPos) const;
    
    // 檢查物件是否在攝像機視野內
    bool IsInView(const Engine::Point& worldPos, float objectRadius = 0) const;
    
    // 獲取可見的地圖範圍
    struct VisibleArea {
        int minX, maxX, minY, maxY;
    };
    VisibleArea GetVisibleTileArea(int tileSize) const;
};