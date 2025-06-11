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
    
    // ³]¸mÄá¹³¾÷¥Ø¼Ð¦ì¸m¡]³q±`¬O¨¤¦â¦ì¸m¡^
    void SetTarget(const Engine::Point& target);
    
    // §ó·sÄá¹³¾÷¦ì¸m¡]¥­·Æ¸òÀH¡^
    void Update(float deltaTime);
    
    // Àò¨úÄá¹³¾÷¦ì¸m
    Engine::Point GetPosition() const { return position; }
    
    // Àò¨úµø¤f¤j¤p
    Engine::Point GetViewportSize() const { return viewportSize; }
    
    // ¥@¬É®y¼ÐÂà´«¬°¿Ã¹õ®y¼Ð
    Engine::Point WorldToScreen(const Engine::Point& worldPos) const;
    
    // ¿Ã¹õ®y¼ÐÂà´«¬°¥@¬É®y¼Ð
    Engine::Point ScreenToWorld(const Engine::Point& screenPos) const;
    
    // ÀË¬dª«¥ó¬O§_¦bÄá¹³¾÷µø³¥¤º
    bool IsInView(const Engine::Point& worldPos, float objectRadius = 0) const;
    
    // Àò¨ú¥i¨£ªº¦a¹Ï½d³ò
=======
    float smoothing;
    
public:
    Camera(float viewportWidth, float viewportHeight);
    
    // è¨­ç½®æ”åƒæ©Ÿç›®æ¨™ä½ç½®ï¼ˆé€šå¸¸æ˜¯è§’è‰²ä½ç½®ï¼‰
    void SetTarget(const Engine::Point& target);
    
    // æ›´æ–°æ”åƒæ©Ÿä½ç½®ï¼ˆå¹³æ»‘è·Ÿéš¨ï¼‰
    void Update(float deltaTime);
    
    // ç²å–æ”åƒæ©Ÿä½ç½®
    Engine::Point GetPosition() const { return position; }
    
    // ç²å–è¦–å£å¤§å°
    Engine::Point GetViewportSize() const { return viewportSize; }
    
    // ä¸–ç•Œåº§æ¨™è½‰æ›ç‚ºèž¢å¹•åº§æ¨™
    Engine::Point WorldToScreen(const Engine::Point& worldPos) const;
    
    // èž¢å¹•åº§æ¨™è½‰æ›ç‚ºä¸–ç•Œåº§æ¨™
    Engine::Point ScreenToWorld(const Engine::Point& screenPos) const;
    
    // æª¢æŸ¥ç‰©ä»¶æ˜¯å¦åœ¨æ”åƒæ©Ÿè¦–é‡Žå…§
    bool IsInView(const Engine::Point& worldPos, float objectRadius = 0) const;
    
    // ç²å–å¯è¦‹çš„åœ°åœ–ç¯„åœ
>>>>>>> 99b35bcf769e3dedb1dc8ef8dc6a0985c2d7badc
    struct VisibleArea {
        int minX, maxX, minY, maxY;
    };
    VisibleArea GetVisibleTileArea(int tileSize) const;
};