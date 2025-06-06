#include "Client.hpp"

#ifndef _WIN32
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#endif

#ifdef _WIN32
#include <ws2tcpip.h>  // 加這行才有 inet_pton() 宣告
#endif

#include <iostream>

GameClient::GameClient() {
#ifdef _WIN32
    WSAStartup(MAKEWORD(2,2), &wsa);
#endif
    sock = INVALID_SOCKET;
}

bool GameClient::connectToServer(const std::string& host, int port) {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connect failed.\n";
        return false;
    }
    std::cout << "Connected to server.\n";
    return true;
}

nlohmann::json GameClient::receiveFrame() {
    static std::string recvBuffer;  // 持久 buffer 保留未完整接收的資料
    char temp[4096];
    int len = recv(sock, temp, sizeof(temp), 0);

    if (len <= 0) {
        std::cerr << "Disconnected or error.\n";
        return {};
    }

    recvBuffer += std::string(temp, len);  // 累積資料

    // ? 安全檢查：避免無窮堆積
    if (recvBuffer.size() > 8192) {
        std::cerr << "[Warning] Buffer overflow, discarding old data.\n";
        recvBuffer = recvBuffer.substr(recvBuffer.size() - 1024);
    }

    // ? 切出每個完整 JSON（用 \n 分隔），只保留最後一個合法的
    size_t pos;
    nlohmann::json latest;
    while ((pos = recvBuffer.find('\n')) != std::string::npos) {
        std::string jsonStr = recvBuffer.substr(0, pos);
        recvBuffer.erase(0, pos + 1);
        try {
            latest = nlohmann::json::parse(jsonStr);
        } catch (...) {
            std::cerr << "Invalid JSON skipped.\n";
        }
    }
    std::cerr<<latest.dump()<<'\n';
    return latest;  // 可能是空的（如果都解析失敗），也可能是最新合法的一幀
}

void GameClient::sendInput(const std::vector<std::string>& keys, int mouseX, int mouseY) {
    nlohmann::json inputJson;
    inputJson["type"] = "input";
    inputJson["keys"] = keys;
    inputJson["mouse"] = { {"x", mouseX}, {"y", mouseY} };

    std::string data = inputJson.dump();
    send(sock, data.c_str(), data.size(), 0);
}

GameClient::~GameClient() {
    closesocket(sock);
#ifdef _WIN32
    WSACleanup();
#endif
}