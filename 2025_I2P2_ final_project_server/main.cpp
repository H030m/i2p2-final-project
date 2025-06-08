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

	//hello
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
? ������]�w�覡�]�ϧΤ����^
�b Server �q���W�A�j�M�uWindows Defender ������v

�I�����u�i���]�w�v

��u��J�W�h�v�� �k���u�s�W�W�h�v

������ܡGPort

TCP�A�S�w���� port�G��J 7000

�ʧ@�G���\�s�u

�]�w�ɡG�����Ŀ�]����B�p�K�B���Ρ^

�W���H�K���@�ӡ]�Ҧp GameServer7000�^
*/