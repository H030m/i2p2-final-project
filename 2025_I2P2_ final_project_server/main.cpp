#include "Engine/GameEngine.hpp"
#include "Engine/LOG.hpp"


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

#include<Connet/RenderSender.hpp>

int main(int argc, char **argv) {
	Engine::LOG::SetConfig(true);
	Engine::GameEngine& game = Engine::GameEngine::GetInstance();

	//偵測會在哪裡跑
	#ifdef _WIN32
    std::cout << "Running on Windows" << std::endl;
	#else
    std::cout << "Running on Linux/Unix" << std::endl;
	#endif

	RenderSender sender(8888);
    sender.start();  // 等待多人連線，並個別傳送畫面
    return 0;

}
/*
? 防火牆設定方式（圖形介面）
在 Server 電腦上，搜尋「Windows Defender 防火牆」

點左側「進階設定」

選「輸入規則」→ 右側「新增規則」

類型選擇：Port

TCP，特定本機 port：輸入 7000

動作：允許連線

設定檔：全部勾選（網域、私密、公用）

名稱隨便取一個（例如 GameServer7000）
*/