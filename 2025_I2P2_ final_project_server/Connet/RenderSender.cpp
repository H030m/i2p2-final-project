// RenderSender.cpp
#include "RenderSender.hpp"

#ifndef _WIN32
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#endif

#include <iostream>
#include <thread>
#include <sys/time.h>  // select �Ρ]Windows �i�@�Ρ^
RenderSender::RenderSender(int port) {
#ifdef _WIN32
    WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
    serverSock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    bind(serverSock, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSock, NUM);
    std::cout << "Listening on port " << port << "...\n";
}

void RenderSender::start() {
    std::cout << "Waiting for clients...\n";
    while (true) {
        SOCKET clientSock = accept(serverSock, nullptr, nullptr);
        if (clientSock == INVALID_SOCKET) continue;

        std::thread([this, clientSock]() {
            handleClient(clientSock);
        }).detach();
    }
}

void RenderSender::handleClient(SOCKET clientSock) {
    std::cout << "Client handler started.\n";

    while (true) {
        // -- 1. ���ե� select �ˬd�O�_����ƥiŪ --
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(clientSock, &readfds);

        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 1000;  // �̦h�� 1ms

        int ready = select(clientSock + 1, &readfds, nullptr, nullptr, &timeout);

        if (ready > 0 && FD_ISSET(clientSock, &readfds)) {
            char buffer[4096] = {};
            int len = recv(clientSock, buffer, sizeof(buffer), 0);
            if (len > 0) {
                std::string msg(buffer, len);
                try {
                    nlohmann::json input = nlohmann::json::parse(msg);
                    if (input["type"] == "input") {
                        std::cout << "[Client Input] Keys: ";
                        for (auto& k : input["keys"]) std::cout << k << " ";
                        std::cout << "\nMouse: (" << input["mouse"]["x"] << ", " << input["mouse"]["y"] << ")\n";
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Invalid JSON received: " << e.what() << "\n";
                }
            } else {
                std::cerr << "Client disconnected or error.\n";
                break;
            }
        }

        // -- 2. �w�ɶǰe�e���]�C�j��@���A��ĳ�t�X FPS ����^ --
        nlohmann::json frame;
        frame["players"] = {
            {{"id", 1}, {"x", 100}, {"y", 200}}
        };
        frame["tiles"] = {
            {{"x", 0}, {"y", 0}, {"type", "GRASS"}}
        };

        std::string out = frame.dump() + "\n";
        send(clientSock, out.c_str(), out.size(), 0);
        std::cout << "Sent frame to client.\n";

        // -- 3. ����ǰe���Z�]�j�� 60FPS = �C�V 16~17ms�^ --
        std::this_thread::sleep_for(std::chrono::milliseconds(1000/TARGET_FPS));
    }

    closesocket(clientSock);
    std::cout << "Client handler closed.\n";
}

RenderSender::~RenderSender() {
    closesocket(serverSock);
#ifdef _WIN32
    WSACleanup();
#endif
}