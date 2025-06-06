#pragma once
#include <string>
#include <nlohmann/json.hpp>

#ifdef _WIN32
#include <winsock2.h>
typedef int socklen_t;
#else
#include <sys/socket.h>
#define SOCKET int
#endif

class GameClient {
public:
    GameClient();
    bool connectToServer(const std::string& host, int port);
    nlohmann::json receiveFrame();
    void sendInput(nlohmann::json& inputJson);
    ~GameClient();

private:
    SOCKET sock;
#ifdef _WIN32
    WSADATA wsa;
#endif
};