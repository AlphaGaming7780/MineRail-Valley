#include <Game/Events/EventManager.hpp>
#include <cassert>

#include <PallasEngine/Logging/PerformanceLogger.hpp>

namespace Game
{
    // ---------- Ctor / Dtor ----------
    EventManager::EventManager()
        : m_Window(nullptr)
    {
    }

    EventManager::~EventManager()
    {
        clear();
        m_Window = nullptr;
    }

    // ---------- Fenêtre ----------
    EventManager& EventManager::initialize(sf::Window& window) noexcept
    {
        m_Window = &window; // non possédé
        return *this;
    }

    void EventManager::resetWindow() noexcept
    {
        m_Window = nullptr;
    }

    // ---------- Process (poll + dispatch) ----------
    void EventManager::process()
    {
        if (!m_Window) return;

        process(*m_Window);
    }

    void EventManager::process(sf::Window& window)
    {
        {
#ifdef DEBUG
            auto perf = pallas::PerformanceLogger::Scoped("EventManager::process");
#endif // DEBUG
            while (auto ev = window.pollEvent())
            {
                Notify(ev.value());
            }
        }
    }

    void EventManager::clear()
    {
        m_Observers.clear();
    }
}