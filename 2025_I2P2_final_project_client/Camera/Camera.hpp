// Camera.hpp
#pragma once
#include "Engine/Point.hpp"

class Camera {
private:
    Engine::Point position;
    Engine::Point viewportSize;
    Engine::Point targetPosition;
<<<<<<< HEAD
    Engine::Point margin;
    float smoothing;
    
public:
    Camera(float viewportWidth, float viewportHeight, Engine::Point margin);
    
    // �]�m�ṳ���ؼЦ�m�]�q�`�O�����m�^
    void SetTarget(const Engine::Point& target);
    
    // ��s�ṳ����m�]���Ƹ��H�^
    void Update(float deltaTime);
    
    // ����ṳ����m
    Engine::Point GetPosition() const { return position; }
    
    // ������f�j�p
    Engine::Point GetViewportSize() const { return viewportSize; }
    
    // �@�ɮy���ഫ���ù��y��
    Engine::Point WorldToScreen(const Engine::Point& worldPos) const;
    
    // �ù��y���ഫ���@�ɮy��
    Engine::Point ScreenToWorld(const Engine::Point& screenPos) const;
    
    // �ˬd����O�_�b�ṳ��������
    bool IsInView(const Engine::Point& worldPos, float objectRadius = 0) const;
    
    // ����i�����a�Ͻd��
=======
    float smoothing;
    
public:
    Camera(float viewportWidth, float viewportHeight);
    
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
>>>>>>> 99b35bcf769e3dedb1dc8ef8dc6a0985c2d7badc
    struct VisibleArea {
        int minX, maxX, minY, maxY;
    };
    VisibleArea GetVisibleTileArea(int tileSize) const;
};