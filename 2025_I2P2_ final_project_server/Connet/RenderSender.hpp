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
    void start();  // �Ұ� server ��ť + �h�H�B�z
    ~RenderSender();

private:
    void handleClient(SOCKET clientSock);  //�� client �B�z�޿� -> ��std::thread�B�z
    SOCKET serverSock;
#ifdef _WIN32
    WSADATA wsa;
#endif
};