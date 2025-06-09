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
#include <ws2tcpip.h>
#endif

#include <iostream>
#include <sys/types.h>

GameClient::GameClient() {
#ifdef _WIN32
    WSAStartup(MAKEWORD(2,2), &wsa);
#endif
    sock = INVALID_SOCKET;
}

bool GameClient::connectToServer(const std::string& host, int port) {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Failed to create socket.\n";
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connect failed.\n";
        return false;
    }

    std::cout << "Connected to server.\n";
    return true;
}

void GameClient::recvOnce() {
    static std::string recvBuffer;

    char temp[4096];
    int len = recv(sock, temp, sizeof(temp), 0);
    if (len <= 0) {
        std::cerr << "Disconnected or error.\n";
        return;
    }

    recvBuffer += std::string(temp, len);

    if (recvBuffer.size() > 8192) {
        std::cerr << "[Warning] Buffer overflow, discarding old data.\n";
        recvBuffer = recvBuffer.substr(recvBuffer.size() - 1024);
    }

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
    input_json = latest;
    std::cerr << "recv " << latest.dump() << '\n';
}

void GameClient::sendOnce() {
    std::string data = output_json.dump() + "\n";
    send(sock, data.c_str(), data.size(), 0);
    // std::cerr << "send " << output_json.dump() << '\n';
}

GameClient::~GameClient() {
    closesocket(sock);
#ifdef _WIN32
    WSACleanup();
#endif
}
