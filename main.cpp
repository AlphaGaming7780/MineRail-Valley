
// Moved into PallasEngine.hpp
// used to force tell the OS we want to run on the main GPU.
//extern "C" {
//    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001; // NVIDIA
//    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;   // AMD
//}

#include <PallasEngine/PallasEngine.hpp>
#include <Game/GameManager.hpp>

using namespace Game;
using namespace pallas;

int main()
{
    // On crée le LogManager en premier pour que ce soit le dernier supprimer.
    //LogManager::Instance();

    PallasEngine::Start<GameManager>();

    // Need to remove that old GameManager and implemente the new one.
    //GameManager& gm = GameManager::getInstance();

    //int x = gm.Setup();

    //if (x != 0) return x;

    //x = gm.Run();

    return 0;
}