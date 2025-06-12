#pragma once

#include <mutex>

class Enemy;
class RenderSender;

void UpdateEnemyInstance(Enemy& enemy, float deltaTime, RenderSender& sender);