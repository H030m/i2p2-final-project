#include "RenderSender.hpp"
#include <iostream>
#include <chrono>
#include <cstring>
#include <thread>
#include <mutex>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <unistd.h>      // close()
    #include <fcntl.h>       // fcntl()
    #include <errno.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif

RenderSender::RenderSender(int port) {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    serverSock =
        socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) {
        perror("socket");
        exit(1);
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(serverSock, NUM) < 0) {
        perror("listen");
        exit(1);
    }

    std::cout << "Listening on port " << port << "...\n";
}

RenderSender::~RenderSender() {
#ifdef _WIN32
    closesocket(serverSock);
    WSACleanup();
#else
    close(serverSock);
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
            auto clientSock = accept(serverSock, nullptr, nullptr);
            if (clientSock < 0) continue;

#ifdef _WIN32
            u_long mode = 1;
            ioctlsocket(clientSock, FIONBIO, &mode);
#else
            int flags = fcntl(clientSock, F_GETFL, 0);
            fcntl(clientSock, F_SETFL, flags | O_NONBLOCK);
#endif
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
        ctx->lastInput = nullptr;

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
        std::cerr << "recv: " << ctx->lastInput << '\n';
    } else if (len == 0) {
        ctx->active = false;
#ifdef _WIN32
        closesocket(ctx->socket);
#else
        close(ctx->socket);
#endif
    } else {
#ifdef _WIN32
        int err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK)
#else
        int err = errno;
        if (err != EWOULDBLOCK && err != EAGAIN)
#endif
        {
            std::cerr << "recv error: " << err << "\n";
            ctx->active = false;
#ifdef _WIN32
            closesocket(ctx->socket);
#else
            close(ctx->socket);
#endif
        }
    }
}

void RenderSender::sendOnce(std::shared_ptr<ClientContext> ctx) {
    if (!ctx->active) return;

    std::string out = frame.dump() + "\n";
    int res = send(ctx->socket, out.c_str(), out.size(), 0);
    std::cerr << "send: " << out << '\n';

    if (res < 0) {
#ifdef _WIN32
        int err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK)
#else
        int err = errno;
        if (err != EWOULDBLOCK && err != EAGAIN)
#endif
        {
            std::cerr << "send error: " << err << "\n";
            ctx->active = false;
#ifdef _WIN32
            closesocket(ctx->socket);
#else
            close(ctx->socket);
#endif
        }
    }
}

void RenderSender::cleanupInactiveClients() {
    std::lock_guard<std::mutex> lock(clientMutex);
    clients.erase(std::remove_if(clients.begin(), clients.end(),
        [](const std::shared_ptr<ClientContext>& ctx) {
            return !ctx->active;
        }), clients.end());
}
