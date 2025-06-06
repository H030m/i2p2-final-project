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

	//�����|�b���̶]
	#ifdef _WIN32
    std::cout << "Running on Windows" << std::endl;
	#else
    std::cout << "Running on Linux/Unix" << std::endl;
	#endif

	RenderSender sender(8888);
    sender.start();  // ���ݦh�H�s�u�A�íӧO�ǰe�e��
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