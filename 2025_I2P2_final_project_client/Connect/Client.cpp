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
#include <stdexcept>
void recvCompressedJson(SOCKET sock, nlohmann::json& outJson);
std::string decompress_string(const std::string& str);
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
char temp[3000000];
void GameClient::recvOnce() {
     bool enable_compression = true;
    try {
        nlohmann::json latest;
        if (enable_compression) {
            recvCompressedJson(sock, latest);
        } else {
            static std::string recvBuffer;
            int len = recv(sock, temp, sizeof(temp), 0);
            if (len <= 0) throw std::runtime_error("Disconnected or error");
            recvBuffer += std::string(temp, len);

            size_t pos;
            while ((pos = recvBuffer.find('\n')) != std::string::npos) {
                std::string jsonStr = recvBuffer.substr(0, pos);
                recvBuffer.erase(0, pos + 1);
                latest = nlohmann::json::parse(jsonStr);
            }
        }
        input_json = latest;
        // std::cerr << "recv: " << latest.dump().size() << '\n';
    } catch (const std::exception& e) {
        std::cerr << "[recvOnce] error: " << e.what() << '\n';
    }
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
void recvCompressedJson(SOCKET sock, nlohmann::json& outJson) {
    uint32_t net_len = 0;
    int ret = recv(sock, (char*)&net_len, sizeof(net_len), MSG_WAITALL);
    if (ret <= 0) throw std::runtime_error("Connection closed or error (reading length)");
    uint32_t data_len = ntohl(net_len);
    std::string buffer(data_len, 0);

    int received = 0;
    while (received < (int)data_len) {
        int n = recv(sock, &buffer[received], data_len - received, 0);
        if (n <= 0) throw std::runtime_error("Connection closed or error (reading body)");
        received += n;
    }

    std::string decompressed = decompress_string(buffer);
    try {
        outJson = nlohmann::json::parse(decompressed);
    } catch (const std::exception& e) {
        std::cerr << "JSON parse failed: " << e.what() << "\n";
        return; // 或保留舊資料
    }
    
}
std::string decompress_string(const std::string& str) {
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if (inflateInit2(&zs, -MAX_WBITS) != Z_OK)
        throw std::runtime_error("inflateInit failed");

    zs.next_in = (Bytef*)str.data();
    zs.avail_in = str.size();

    int ret;
    char outbuffer[32768];
    std::string outstring;

    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = inflate(&zs, 0);

        if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
            inflateEnd(&zs);
            throw std::runtime_error("inflate failed with error code " + std::to_string(ret));
        }

        outstring.append(outbuffer, sizeof(outbuffer) - zs.avail_out);
    } while (ret != Z_STREAM_END);

    inflateEnd(&zs);
    return outstring;
}