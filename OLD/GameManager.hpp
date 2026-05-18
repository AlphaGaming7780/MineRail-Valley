#pragma once

#include <SFML/Graphics.hpp>
#include <PallasEngine/PallasEngine.hpp>

using namespace pallas;

namespace Game {
    class GameManager
    {
    public:
        ~GameManager();

        int Setup();

        int Run();

        void UpdateView(sf::Vector2u size);

        static GameManager& getInstance();

    private:

        sf::RenderWindow window;
        sf::View view;

        World& m_World = World::Instance();

        AssetDatabase& db = AssetDatabase::Instance();

        Logger m_Logger = Logger("GameManager");

        GameManager();
        GameManager(const GameManager&) = delete;
        GameManager& operator=(const GameManager&) = delete;
    };
}