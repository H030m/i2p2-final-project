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
#include "Scene/DrawMapScene.hpp"
#include "Scene/LoadingScene.hpp"
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

#include<Connect/Client.hpp>
#include<Scene/RemotePlayScene.hpp>
#include "UI/Component/Label.hpp"     
#include "Engine/GameEngine.hpp"

int main(int argc, char **argv) {
	Engine::LOG::SetConfig(true);
	Engine::GameEngine& game = Engine::GameEngine::GetInstance();
	game.AddNewScene("DrawMapScene", new DrawMapScene());
	game.AddNewScene("stage-select", new StageSelectScene());
	game.AddNewScene("play", new PlayScene());
	game.AddNewScene("lose", new LoseScene());
	game.AddNewScene("win", new WinScene());
	game.AddNewScene("settings", new SettingsScene());
	game.AddNewScene("scoreboard-scene", new ScoreboardScene());
	game.AddNewScene("submit-scene", new SubmitScene());
	game.AddNewScene("start", new StartScene());
	game.AddNewScene("loading", new LoadingScene());
	game.Start("start", 60, 1600, 832);
	
	return 0;
}
