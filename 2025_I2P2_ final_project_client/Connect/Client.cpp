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
#include <sys/types.h>
#include <sys/time.h>

GameClient::GameClient() {
#ifdef _WIN32
    WSAStartup(MAKEWORD(2,2), &wsa);
#endif
    sock = INVALID_SOCKET;
}

bool GameClient::setNonBlocking() {
#ifdef _WIN32
    u_long mode = 1;
    return ioctlsocket(sock, FIONBIO, &mode) == 0;
#else
    int flags = fcntl(sock, F_GETFL, 0);
    return fcntl(sock, F_SETFL, flags | O_NONBLOCK) == 0;
#endif
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
    
    int result = connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (!setNonBlocking()) {
        std::cerr << "Failed to set non-blocking.\n";
        return false;
    }
    #ifdef _WIN32
        if (result == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK && WSAGetLastError() != WSAEINPROGRESS) {
            std::cerr << "Connect failed.\n";
            return false;
        }
    #else
        if (result < 0 && errno != EINPROGRESS) {
            std::cerr << "Connect failed.\n";
            return false;
        }
    #endif
            
    std::cout << "Connecting...\n";

    // wait for connecting
    fd_set writeSet;
    FD_ZERO(&writeSet);
    FD_SET(sock, &writeSet);
    timeval timeout{3, 0};// connecting in 3 second
    
    if (select(sock + 1, nullptr, &writeSet, nullptr, &timeout) <= 0) {
        std::cerr << "Connect timeout.\n";
        return false;
    }

    std::cout << "Connected to server.\n";
    return true;
}

void GameClient::recvOnce() {
    static std::string recvBuffer;
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(sock, &readSet);
    timeval timeout{0, 0};  // non-blocking select

    // check if socket is readable
    if (select(sock + 1, &readSet, nullptr, nullptr, &timeout) > 0 && FD_ISSET(sock, &readSet)) {
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
        
    }
}

// Pass in a json file, and it will automatically send it to you.
void GameClient::sendOnce() {
    fd_set writeSet;
    FD_ZERO(&writeSet);
    FD_SET(sock, &writeSet);
    timeval timeout{0, 0};  // non-blocking select

    if (select(sock + 1, nullptr, &writeSet, nullptr, &timeout) > 0 && FD_ISSET(sock, &writeSet)) {
        std::string data = output_json.dump() + "\n";
        send(sock, data.c_str(), data.size(), 0);
        // std::cerr<<"send "<<output_json.dump()<<'\n';
    }

}

GameClient::~GameClient() {
    closesocket(sock);
#ifdef _WIN32
    WSACleanup();
#endif
}