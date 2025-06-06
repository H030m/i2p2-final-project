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
    char buffer[4096] = {};
    int len = recv(sock, buffer, sizeof(buffer), 0);
    if (len <= 0) {
        std::cerr << "Disconnected or error.\n";
        return {};
    }
    std::string msg(buffer, len);
    std::cout<<buffer<<'\n';
    return nlohmann::json::parse(msg);
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