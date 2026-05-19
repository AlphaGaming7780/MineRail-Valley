#include <PallasEngine/Events/EventManager.hpp>
#include <cassert>

#include <PallasEngine/Logging/PerformanceLogger.hpp>

namespace pallas
{
    // ---------- Ctor / Dtor ----------
    EventManager::EventManager()
        : m_window(nullptr)
        , m_nextId(1)
        , m_dispatchDepth(0)
    {
    }

    EventManager::~EventManager()
    {
        clear();
        m_window = nullptr;
    }

    // ---------- Fenêtre ----------
    EventManager& EventManager::initialize(sf::Window& window) noexcept
    {
        m_window = &window; // non possédé
        return *this;
    }

    void EventManager::resetWindow() noexcept
    {
        m_window = nullptr;
    }

    // ---------- Process (poll + dispatch) ----------
    void EventManager::process()
    {
        if (!m_window) return;

        process(*m_window);
    }

    void EventManager::process(sf::Window& window)
    {
        {
#ifdef DEBUG
            auto perf = PerformanceLogger::Scoped("EventManager::process");
#endif // DEBUG
            while (auto ev = window.pollEvent())
            {
                dispatch(ev.value());
            }
        }

        if (m_dirty && m_dispatchDepth == 0) {
            sweepGarbage();               // sweep une fois par frame (si nécessaire)
            m_dirty = false;
        }
    }

    // ---------- Abonnements ----------
    EventManager::ListenerId EventManager::onAny(AnyListener cb)
    {
        const auto id = m_nextId++;
        SlotAny s{ id, std::move(cb), true };
        m_any.emplace_back(std::move(s));
        m_index.emplace(id, Location{ /*isAny=*/true, std::type_index(typeid(void)) });
        return id;
    }

    bool EventManager::off(ListenerId id)
    {
        auto it = m_index.find(id);
        if (it == m_index.end()) return false;

        const Location loc = it->second;

        auto deactivateTyped = [this, id](SlotsTyped& slots) -> bool 
        {
            for (auto& slot : slots)
            {
                if (slot.id == id && slot.active)
                {
                    if (m_dispatchDepth > 0) {
                        slot.active = false;
                        m_dirty = true;
                    }
                    else {
                        slot = std::move(slots.back());
                        slots.pop_back();
                    }
                    return true;
                }
            }
            return false;
        };
        auto deactivateAny = [this, id]() -> bool 
        {
            for (auto& slot : m_any)
            {
                if (slot.id == id && slot.active)
                {
                    if (m_dispatchDepth > 0) {
                        slot.active = false;
                        m_dirty = true;
                    }
                    else {
                        slot = std::move(m_any.back());
                        m_any.pop_back();
                    }
                    return true;
                }
            }
            return false;
        };

        bool ok = false;
        if (loc.isAny) {
            ok = deactivateAny();
        }
        else {
            auto itVec = m_byType.find(loc.key);
            if (itVec != m_byType.end()) {
                ok = deactivateTyped(itVec->second);
                // pas de suppression de la clé vide ici ; pas nécessaire
            }
        }

        if (ok) m_index.erase(it);
        return ok;
    }

    void EventManager::clear()
    {
        m_byType.clear();
        m_any.clear();
        m_index.clear();
        // On conserve m_nextId pour garantir l’unicité des ids dans le temps.
    }

    void EventManager::emitToTyped(std::type_index key, const void* payload)
    {
        auto it = m_byType.find(key);
        if (it == m_byType.end()) return;

        auto& slots = it->second;
        const std::size_t n = slots.size();
        for (std::size_t i = 0; i < n; ++i)
        {
            auto& s = slots[i];
            if (s.active && s.func) {
                s.func(payload);
            }
        }
    }

    void EventManager::emitToAny(const sf::Event& ev)
    {
        const std::size_t n = m_any.size();
        for (std::size_t i = 0; i < n; ++i)
        {
            auto& s = m_any[i];
            if (s.active && s.func) {
                s.func(ev);
            }
        }
    }

    void EventManager::sweepGarbage()
    {
#ifdef DEBUG
        auto perf = PerformanceLogger::Scoped("EventManager::sweepGarbage");
#endif // DEBUG
        auto sweepTyped = [this](SlotsTyped& slots) {
            std::size_t w = 0;
            for (std::size_t r = 0; r < slots.size(); ++r) {
                if (slots[r].active) {
                    if (w != r) slots[w] = std::move(slots[r]);
                    ++w;
                }
                else {
                    m_index.erase(slots[r].id);
                }
            }
            slots.resize(w);
        };

        auto sweepAny = [this](SlotsAny& slots) {
            std::size_t w = 0;
            for (std::size_t r = 0; r < slots.size(); ++r) {
                if (slots[r].active) {
                    if (w != r) slots[w] = std::move(slots[r]);
                    ++w;
                }
                else {
                    m_index.erase(slots[r].id);
                }
            }
            slots.resize(w);
        };

        for (auto& [_, vec] : m_byType) {
            sweepTyped(vec);
        }

        sweepAny(m_any);
    }

    // -----
    // NOTE IMPORTANTE :
    // Les méthodes template doivent être visibles au point d'appel.
    // Si tu sépares .hpp/.cpp, garde le template `on<TEvent>` dans le .hpp
    // OU fournis une implémentation inline comme ici via inclusion du .cpp à la fin du .hpp.
    // -----
}