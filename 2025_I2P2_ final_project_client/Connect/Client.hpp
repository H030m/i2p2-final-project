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
    nlohmann::json input_json,output_json;
    GameClient();
    bool connectToServer(const std::string& host, int port);
    // bool setNonBlocking();
    void recvOnce();
    void sendOnce();
    ~GameClient();

private:
    SOCKET sock;
#ifdef _WIN32
    WSADATA wsa;
#endif
};