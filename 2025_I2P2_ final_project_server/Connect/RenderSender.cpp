// RenderSender.cpp
#include "RenderSender.hpp"
#include <iostream>
#include <chrono>

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

RenderSender::~RenderSender() {
    closesocket(serverSock);
#ifdef _WIN32
    WSACleanup();
#endif

    for (auto& ctx : clients) {
        ctx->active = false;
        if (ctx->recvThread.joinable()) ctx->recvThread.join();
        if (ctx->sendThread.joinable()) ctx->sendThread.join();
    }
}

void RenderSender::start() {
    std::thread([this]() {
        std::cout << "Client accept thread started.\n";
        while (true) {
            SOCKET clientSock = accept(serverSock, nullptr, nullptr);
            u_long mode = 1;
            ioctlsocket(clientSock, FIONBIO, &mode);
            if (clientSock == INVALID_SOCKET) continue;

            auto context = std::make_shared<ClientContext>();
            context->socket = clientSock;

            {
                std::lock_guard<std::mutex> lock(clientMutex);
                clients.push_back(context);
            }
        }
    }).detach();
}

void RenderSender::recvOnce(std::shared_ptr<ClientContext> ctx) {
    char buffer[4096] = {};
    int len = recv(ctx->socket, buffer, sizeof(buffer), 0);
    
    if (len > 0) {
        std::string raw(buffer, len);
        size_t start = 0;
        ctx->lastInput = nullptr; // 清空先前訊息

        while (start < raw.size()) {
            size_t end = raw.find('\n', start);
            if (end == std::string::npos) break;
            std::string line = raw.substr(start, end - start);
            start = end + 1;
            try {
                ctx->lastInput = nlohmann::json::parse(line);
            } catch (...) {
                std::cerr << "Invalid JSON chunk from client.\n";
            }
        }
        // std::cerr<<"recv: "<<ctx->lastInput<<'\n';
    } else if (len == 0) {
        // client 正常關閉連線
        ctx->active = false;
        closesocket(ctx->socket);
    } else {
        int err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) {
            // 真正錯誤才關閉
            std::cerr << "recv error: " << err << "\n";
            ctx->active = false;
            closesocket(ctx->socket);
        }
        // 否則是沒有資料，不做事
    }
}
void RenderSender::sendOnce(std::shared_ptr<ClientContext> ctx) {
    if (!ctx->active) return;

    std::string out = frame.dump() + "\n";
    int res = send(ctx->socket, out.c_str(), out.size(), 0);
    std::cerr<<"send: "<<out<<'\n';
    if (res == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) {
            std::cerr << "send error: " << err << "\n";
            ctx->active = false;
            closesocket(ctx->socket);
        }
        // 否則只是不能立即送，不處理
    }
}

void RenderSender::cleanupInactiveClients() {
    std::lock_guard<std::mutex> lock(clientMutex);
    clients.erase(std::remove_if(clients.begin(), clients.end(),
        [](const std::shared_ptr<ClientContext>& ctx) {
            return !ctx->active;
        }), clients.end());
}