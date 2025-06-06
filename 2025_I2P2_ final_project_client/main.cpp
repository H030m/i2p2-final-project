// [main.cpp]
// This is the entry point of your game.
// You can register your scenes here, and start the game.
#include "Engine/GameEngine.hpp"
#include "Engine/LOG.hpp"
#include "Scene/LoseScene.hpp"
#include "Scene/PlayScene.hpp"
#include "Scene/StageSelectScene.hpp"
#include "Scene/WinScene.hpp"
#include "Scene/StartScene.h"
#include "Scene/SettingsScene.hpp"
#include "Scene/ScoreboardScene.hpp"
#include "Scene/SubmitScene.hpp"

//Json
#include <nlohmann/json.hpp>
using json = nlohmann::json;

//Socket
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

#include<Connet/Client.hpp>
const std::string host = "140.114.196.15";
int main(int argc, char **argv) {
	Engine::LOG::SetConfig(true);
	Engine::GameEngine& game = Engine::GameEngine::GetInstance();
	
	GameClient client;
    if (!client.connectToServer(host, 8888))
        return 1;

    std::vector<std::string> keys;
    std::string key;
    std::cout << "Press key (W/A/S/D), type END to stop: ";
    while (std::cin >> key && key != "END") {
        keys.push_back(key);
		client.sendInput(keys, 0, 0);
		client.receiveFrame();
    }

    int mouseX = 0, mouseY = 0;
    std::cout << "Mouse X Y: ";
    std::cin >> mouseX >> mouseY;

    client.sendInput(keys, mouseX, mouseY);

    return 0;

	return 0;
}
