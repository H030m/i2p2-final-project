// #include <allegro5/base.h>
// #include <random>
// #include <string>

// #include "Engine/Point.hpp"
// #include "BossEnemy.hpp"

// #include "Scene/PlayScene.hpp"
// #include "UI/Animation/DirtyEffect.hpp"
// #include "Engine/Group.hpp"
// #include "Engine/Point.hpp"
// #include "Turret/Turret.hpp"
// const float SkillInitTime = 3;
// const float SkillGap = 5;

// BossEnemy::BossEnemy(int x, int y)
//     : Enemy("play/enemy-6.png", x, y, 20, 100, 1000, 100) {
//         skill_time = SkillInitTime;
// }
// void BossEnemy::Draw() const {
//     Enemy::Draw();
// }
// void BossEnemy::Update(float deltaTime) {
//     Enemy::Update(deltaTime);
//     skill_time -= deltaTime;
//     while(skill_time <= 0){
//         skill_time += SkillGap;
//         // healing
//         DirtyEffect *dirty = new DirtyEffect("play/healing.png", 5,Position.x, Position.y, false);
//         dirty->Size.x = 300;
//         dirty->Size.y = 300;
//         getPlayScene()->GroundEffectGroup->AddNewObject(dirty);

//         // heal enemy
//         auto enemy_list = getPlayScene()->EnemyGroup->GetObjects();
//         for(auto it = enemy_list.begin(); it != enemy_list.end(); it++){
//             Enemy *enemy = dynamic_cast<Enemy *>(*it);
//             if((enemy->Position.x - Position.x)*(enemy->Position.x - Position.x) + 
//                (enemy->Position.y - Position.y)*(enemy->Position.y - Position.y) <= 22500){
//                     enemy->hp += 20;
//                     enemy->poison = false;
//                     if(enemy->tag)
//                     enemy->speed += 20,enemy->tag = 0;
//                }
//         }
//         // auto tower = getPlayScene()->TowerGroup->GetObjects();
//         // for(auto it = tower.begin(); it != tower.end(); it++){
//         //     Engine::Point p = (*it)->Position;
//         //     if((p.x - Position.x)*(p.x - Position.x) + 
//         //        (p.y - Position.y)*(p.y - Position.y) <= 100000){
//         //              (*it)->GetObjectIterator()->first = false;
//         //             getPlayScene()->TowerGroup->RemoveObject((*it)->GetObjectIterator());
//         //             break;
//         //        }
//         // }
//         auto tower = getPlayScene()->TowerGroup->GetObjects();
//         for(auto it = tower.begin(); it != tower.end(); it++){
//             Turret *tower = dynamic_cast<Turret*>(*it);
//             if((tower->Position.x - Position.x)*(tower->Position.x - Position.x) + 
//                (tower->Position.y - Position.y)*(tower->Position.y - Position.y) <= 22500){
//                     tower->puase_time = 1;
                    
//                     break;
//                }
//         }
//     }
// }
