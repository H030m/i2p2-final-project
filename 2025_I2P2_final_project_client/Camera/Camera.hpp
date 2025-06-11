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
    struct VisibleArea {
        int minX, maxX, minY, maxY;
    };
    VisibleArea GetVisibleTileArea(int tileSize) const;
};