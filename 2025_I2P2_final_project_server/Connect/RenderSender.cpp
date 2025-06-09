// RenderSender.cpp
#include "RenderSender.hpp"
#include <iostream>
#include <chrono>
std::string compress_string(const std::string& str);
std::string decompress_string(const std::string& str, uLong estimated_size = 10 * 1024 * 1024);
void sendCompressedJson(SOCKET sock, const nlohmann::json& j);

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
char buffer[1000000];
void RenderSender::recvOnce(std::shared_ptr<ClientContext> ctx) {
    
    int len = recv(ctx->socket, buffer, sizeof(buffer), 0);
    
    if (len > 0) {
        std::string raw(buffer, len);
        size_t start = 0;
        ctx->lastInput = nullptr; // Clear previous messages

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
        std::cerr<<"recv: "<<ctx->lastInput<<'\n';
    } else if (len == 0) {
        // client Close the connection normally
        ctx->active = false;
        closesocket(ctx->socket);
    } else {
        int err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) {
            // Close only if it is a real error
            std::cerr << "recv error: " << err << "\n";
            ctx->active = false;
            closesocket(ctx->socket);
        }
        // Otherwise, there is no information and no work.
    }
}
void RenderSender::sendOnce(std::shared_ptr<ClientContext> ctx) {
    if (!ctx->active) return;
    // std::cerr<<"framesize "<<frame.dump().size()<<'\n';
    std::string raw = frame.dump(); // ­ì©l JSON ¦r¦ê
    // std::cerr << "[send raw size]: " << raw.size() << " bytes\n";
    // std::cerr << "[send raw content]: " << raw << "\n";

    try {
        std::string compressed = compress_string(raw);
        uint32_t len = htonl(compressed.size());

        int header_sent = send(ctx->socket, (char*)&len, sizeof(len), 0);
        int body_sent = send(ctx->socket, compressed.data(), compressed.size(), 0);

        if (header_sent == SOCKET_ERROR || body_sent == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err != WSAEWOULDBLOCK) {
                std::cerr << "send error: " << err << "\n";
                ctx->active = false;
                closesocket(ctx->socket);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Compression or send failed: " << e.what() << "\n";
    }
}

void RenderSender::cleanupInactiveClients() {
    std::lock_guard<std::mutex> lock(clientMutex);
    clients.erase(std::remove_if(clients.begin(), clients.end(),
        [](const std::shared_ptr<ClientContext>& ctx) {
            return !ctx->active;
        }), clients.end());
}

std::string compress_string(const std::string& str) {
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if (deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK)
        throw std::runtime_error("deflateInit failed");

    zs.next_in = (Bytef*)str.data();
    zs.avail_in = str.size();

    int ret;
    char outbuffer[32768];
    std::string outstring;

    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = deflate(&zs, Z_FINISH);

        if (ret == Z_STREAM_ERROR) {
            deflateEnd(&zs);
            throw std::runtime_error("deflate failed");
        }

        outstring.append(outbuffer, sizeof(outbuffer) - zs.avail_out);
    } while (ret != Z_STREAM_END);

    deflateEnd(&zs);
    return outstring;
}

std::string decompress_string(const std::string& str, uLong estimated_size) {
    std::string out(estimated_size, 0);
    uLong destLen = out.size();
    if (uncompress((Bytef*)&out[0], &destLen, (const Bytef*)str.data(), str.size()) != Z_OK) {
        throw std::runtime_error("Decompression failed.");
    }
    out.resize(destLen);
    return out;
}

void sendCompressedJson(SOCKET sock, const nlohmann::json& j) {
    std::string raw = j.dump();
    std::string compressed = compress_string(raw);
    uint32_t len = htonl(compressed.size());
    send(sock, (char*)&len, sizeof(len), 0);
    send(sock, compressed.data(), compressed.size(), 0);
}