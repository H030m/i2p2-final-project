#include <iostream>

#include "Connect/RenderSender.hpp"

int main(int argc, char **argv) {
	#ifdef _WIN32
    std::cout << "Running on Windows" << std::endl;
	#else
    std::cout << "Running on Linux/Unix" << std::endl;
	#endif

	RenderSender rendersender(8888);
	rendersender.start();
	
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