// RenderSender.hpp
#pragma once

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#endif

#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <thread>
#include <optional>
#include <zlib.h>
#include "Enemy/Enemy.hpp"
#define TARGET_FPS 60
#define NUM 5
constexpr int blockSize = 64;
struct ClientContext {
        SOCKET socket;
        bool active = true;
        nlohmann::json lastInput;
        std::thread recvThread;
        std::thread sendThread;
        int id;
        int enemy_id;
        int x, y;
        ClientContext(){
            x = 0, y = 0;
        }
        //camera position
        bool sendMap = false;
};
struct HitInformation{
    int damage;
    float HitVX;
    float HitVY;
    int player_id;
    HitInformation(int damage, float HitVX, float HitVY, int player_id);
};

class RenderSender {
public:
    static constexpr int TILE_SIZE = 64;
    RenderSender(int port);
    RenderSender() = default;
    ~RenderSender();

    void start();
    void recvOnce(std::shared_ptr<ClientContext> ctx);
    void sendOnce(std::shared_ptr<ClientContext> ctx);
    void cleanupInactiveClients();
    void AddToFrame(const nlohmann::json& object) {
        std::lock_guard<std::mutex> lock(clientMutex);
        frame[object["type"]].push_back(object);
    }
    friend void UpdateEnemyInstance(Enemy& enemy, float deltaTime, RenderSender& sender);
    std::vector<Enemy*> enemies;
    std::unordered_map<int,std::vector<HitInformation>>Hitenemy;
    std::vector<std::shared_ptr<ClientContext>>& getClients() { return clients; }
    nlohmann::json frame;
    std::vector<std::shared_ptr<ClientContext>> clients;
    std::optional<nlohmann::json> storedMapState;
    std::mutex clientMutex;
    int nextClientId = 1; 
    
private:

    SOCKET serverSock;
#ifdef _WIN32
    WSADATA wsa;
#endif
    
    
};
