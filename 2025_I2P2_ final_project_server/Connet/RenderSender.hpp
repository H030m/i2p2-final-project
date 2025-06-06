#pragma once

#include <nlohmann/json.hpp>

#ifdef _WIN32
#include <winsock2.h>
typedef int socklen_t;
#else
#include <sys/socket.h>
#define SOCKET int
#endif

const int NUM = 2;// how many people can play together

class RenderSender {
public:
    RenderSender(int port);
    void start();  // 啟動 server 監聽 + 多人處理
    ~RenderSender();

private:
    void handleClient(SOCKET clientSock);  //單 client 處理邏輯 -> 用std::thread處理
    SOCKET serverSock;
#ifdef _WIN32
    WSADATA wsa;
#endif
};