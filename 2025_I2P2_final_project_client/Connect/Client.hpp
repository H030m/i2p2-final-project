#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <nlohmann/json.hpp>

#ifdef _WIN32
#include <winsock2.h>
typedef SOCKET socket_t;
#else
typedef int socket_t;
#endif

class GameClient {
public:
    GameClient();
    ~GameClient();

    bool connectToServer(const std::string& host, int port);
    void recvOnce();
    void sendOnce();

    nlohmann::json input_json;
    nlohmann::json output_json;

private:
#ifdef _WIN32
    WSADATA wsa;
#endif
    socket_t sock;
};

#endif // CLIENT_HPP