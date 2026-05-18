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

namespace pallas
{
    class EventManager
    {
    public:
        using ListenerId = std::uint32_t;

        // Listeners "any" : reçoivent l'Event brut (le variant)
        using AnyListener = std::function<void(const sf::Event&)>;

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

        // --- Abonnements ---
        // S'abonner à un type précis d'événement : ex. on<sf::Event::KeyPressed>(...)
        template <class TEvent>
        ListenerId on(std::function<void(const TEvent&)> cb)
        {
            static_assert(!std::is_same_v<TEvent, sf::Event>,
                "Utilise onAny() pour recevoir tous les sf::Event (variant).");

            const auto id = m_nextId++;
            const auto key = typeKey<TEvent>();

            SlotTyped s;
            s.id = id;
            s.active = true;
            // Type-erasure : on projette (const void*) -> (const TEvent&)
            s.func = [fn = std::move(cb)](const void* payload) {
                const TEvent& ev = *static_cast<const TEvent*>(payload);
                fn(ev);
                };

            m_byType[key].emplace_back(std::move(s));
            m_index.emplace(id, Location{ /*isAny=*/false, key });
            return id;
        }

        // S'abonner à tous les événements
        ListenerId onAny(AnyListener cb);

        // Désinscription par id
        bool off(ListenerId id);

        // Tout nettoyer
        void clear();

        // Dispatch manuel
        void dispatch(const sf::Event& ev);

    private:
        EventManager();

        // Slot typé : on stocke un callback type-erasé qui reçoit un pointeur sur l'Instance concrète
        struct SlotTyped {
            ListenerId id = 0;
            std::function<void(const void*)> func; // reçoit &TEvent
            bool active = false;
        };

        struct SlotAny {
            ListenerId id = 0;
            AnyListener func; // reçoit le variant sf::Event
            bool active = false;
        };

        using SlotsTyped = std::vector<SlotTyped>;
        using SlotsAny = std::vector<SlotAny>;

        // Helpers
        template <class TEvent>
        static std::type_index typeKey() noexcept { return std::type_index(typeid(TEvent)); }

        void emitToTyped(std::type_index key, const void* payload);
        void emitToAny(const sf::Event& ev);
        void sweepGarbage(); // supprime les slots désactivés si pas en phase de dispatch

        // État
        sf::Window* m_window = nullptr; // non possédé

        // Map : type d'événement -> slots
        std::unordered_map<std::type_index, SlotsTyped> m_byType;
        SlotsAny m_any;

        // Pour off(id) O(1)
        struct Location {
            bool isAny;
            std::type_index key; // si isAny = true, key = typeid(void)
        };
        std::unordered_map<ListenerId, Location> m_index;

        ListenerId m_nextId;
        int        m_dispatchDepth; // >0 pendant l’émission → désinscriptions différées
        bool       m_dirty = false;
    };
}