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