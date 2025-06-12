#ifndef ENEMY_INSTANCE_HPP
#define ENEMY_INSTANCE_HPP

#include <vector>
#include <cmath>

struct Point {
    float x, y;
    Point() : x(0), y(0) {}
    Point(float x, float y) : x(x), y(y) {}

    Point operator+(const Point& rhs) const { return Point(x + rhs.x, y + rhs.y); }
    Point operator-(const Point& rhs) const { return Point(x - rhs.x, y - rhs.y); }
    Point operator*(float s) const { return Point(x * s, y * s); }
    float Magnitude() const { return std::sqrt(x * x + y * y); }
    Point Normalize() const {
        float mag = Magnitude();
        return (mag == 0) ? Point(0, 0) : Point(x / mag, y / mag);
    }
};

class RenderSender; // forward declare

struct EnemyInstance {
    int id;
    int type;
    int spawnX, spawnY;
    float x, y;
    float cooldownTimer = 0;
    bool alive = true;
    float speed = 100.0f;

    std::vector<Point> path;
    Point velocity;
    float rotation = 0;
    float reachEndTime = 0;

    friend void UpdateEnemyInstance(EnemyInstance& enemy, float deltaTime, RenderSender& sender);
};

#endif