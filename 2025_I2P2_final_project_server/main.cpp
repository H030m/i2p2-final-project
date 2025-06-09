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



int main(int argc, char **argv) {
	Engine::LOG::SetConfig(true);
	Engine::GameEngine& game = Engine::GameEngine::GetInstance();

	//偵測會在哪裡跑
	#ifdef _WIN32
    std::cout << "Running on Windows" << std::endl;
	#else
    std::cout << "Running on Linux/Unix" << std::endl;
	#endif



    game.AddNewScene("stage-select", new StageSelectScene());
	game.AddNewScene("play", new PlayScene());
	game.AddNewScene("lose", new LoseScene());
	game.AddNewScene("win", new WinScene());
	game.AddNewScene("settings", new SettingsScene());
	game.AddNewScene("scoreboard-scene", new ScoreboardScene());
	game.AddNewScene("submit-scene", new SubmitScene());
	game.AddNewScene("start", new StartScene());
	game.Start("start", 60, 1600, 832);
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