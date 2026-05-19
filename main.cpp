
// Moved into PallasEngine.hpp
// used to force tell the OS we want to run on the main GPU.
//extern "C" {
//    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001; // NVIDIA
//    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;   // AMD
//}

#include <Game/GameManager.hpp>

int main()
{
	return Game::GameManager::Instance().Start();
}