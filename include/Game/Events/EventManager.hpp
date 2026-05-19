#pragma once

#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>
#include <cstdint>
#include <vector>
#include <functional>
#include <unordered_map>
#include <typeindex>   // std::type_index
#include <type_traits> // std::is_same_v
#include <utility>     // std::move
#include <variant>     // pour std::visit sur sf::Event

#include <Game/Events/IEventObserver.hpp>

#include <PallasEngine/Logging.hpp>


namespace Game
{
    class EventManager
    {
    public:

        ~EventManager();

        // Singleton
        static EventManager& Instance() {
            static EventManager inst;
            return inst;
        }

        EventManager(const EventManager&) = delete;
        EventManager& operator=(const EventManager&) = delete;

        // --- Fenêtre ---
        // Non possédé : la durée de vie doit être gérée ailleurs.
        EventManager& initialize(sf::Window& window) noexcept;
        void resetWindow() noexcept;

        // --- Boucle d'événements ---
        void process();                 // utilise la fenêtre enregistrée
        void process(sf::Window& window); // stateless

        template<class TEvent>
        void Register(IEventObserver<TEvent>* eventHandler)
        {
            if (m_IsNotifying)
            {
                m_Logger.ErrorO("Can't register EventObserver durint notifying.");
                return;
            }

            const auto key = typeKey<TEvent>();
            m_Observers[key].push_back(eventHandler);
        }

        template<class TEvent>
        void Register(IEventObserver<TEvent>& eventHandler)
        {
            Register<TEvent>(&eventHandler);
        }

        template<class TEvent>
        bool UnRegister(IEventObserver<TEvent>* eventHandler)
        {

            if (m_IsNotifying)
            {
                m_Logger.ErrorO("Can't unregister EventObserver durint notifying.");
                return;
            }

            const auto key = typeKey<TEvent>();
            auto it = m_Observers.find(key);
            if (it == m_Observers.end())
                return false;

            std::vector<IEventObserverBase*>& vec = it->second;
            auto oldSize = vec.size();

            vec.erase(
                std::remove(vec.begin(), vec.end(), eventHandler),
                vec.end()
            );

            return vec.size() != oldSize;
        }

        template<class TEvent>
        bool UnRegister(IEventObserver<TEvent>& eventHandler)
        {
            return UnRegister<TEvent>(&eventHandler)
        }

        // Tout nettoyer
        void clear();

        template<typename T>
        void Notify(const T& event)
        {
            m_IsNotifying = true;
            auto it = m_Observers.find(typeid(T));
            if (it == m_Observers.end())
                return;

            for (IEventObserverBase* baseObs : it->second)
            {
                auto* obs = static_cast<IEventObserver<T>*>(baseObs);
                obs->OnEvent(event);
            }
            m_IsNotifying = false;
        }

    private:
        EventManager();

        // Helpers
        template <class TEvent>
        static std::type_index typeKey() noexcept { return std::type_index(typeid(TEvent)); }

        pallas::Logger m_Logger = pallas::Logger("EventManager");

        // État
        sf::Window* m_Window = nullptr; // non possédé

        bool m_IsNotifying = false;

        // Map : type d'événement -> Observers
        std::unordered_map<std::type_index, std::vector<IEventObserverBase*>> m_Observers;
    };
}