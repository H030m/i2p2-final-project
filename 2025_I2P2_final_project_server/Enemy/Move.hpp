#pragma once

#include <mutex>
#include <vector>
#include <Connect/RenderSender.hpp>
class Enemy;
class RenderSender;

void UpdateEnemyInstance(Enemy& enemy, float deltaTime, RenderSender& sender);
void UpdateEnemyHit(Enemy& enemy, float deltaTime, RenderSender& sender, std::vector<HitInformation>& hit_information);