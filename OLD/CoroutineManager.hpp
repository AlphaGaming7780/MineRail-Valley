#pragma once
#include <vector>
#include <memory>
#include <future>
#include <chrono>
#include <utility>
#include <algorithm>

#include <PallasEngine/Utils/Time.hpp>

/*
    Peut être upgarde de c++ 17 a c++ 20 pour ca, car il offre plein "truc" déjà fait (co_await, co_resturn, co_yield, ...).
    J'ai Update le projet en c++ 20, a Update pour utilser les nouvelle feature.
*/

namespace pallas 
{
    // ─────────────────────────────────────────────
// Interface de coroutine coopérative
// tick(dt) -> false tant que la coroutine vit ; true quand elle est terminée
    struct ICoroutine {
        virtual ~ICoroutine() = default;
        virtual bool tick(float dt) = 0;
    };

    // Champs “cachés” pour l’état : numéro d’étape, attente en secondes, frames
#define CORO_FIELDS        \
    int   _state  = 0;     \
    float _wait   = 0.0f;  \
    int   _frames = 0;

// Début/fin d’une coroutine (state machine via switch + __LINE__)
#define CORO_BEGIN switch (_state) { case 0:
#define CORO_END   } return true;

// Yield “simple” : on sort, on reprendra à cet endroit au prochain tick
#define CORO_YIELD() \
    do { _state = __LINE__; return false; case __LINE__:; } while (0)

// Attente en secondes (dt cumulé côté coroutine)
#define CORO_WAIT_SECONDS(sec) \
    do { _wait = (sec); _state = __LINE__; return false; case __LINE__: if ((_wait -= dt) > 0.0f) return false; } while (0)

// Attente en nombre de frames
#define CORO_WAIT_FRAMES(n) \
    do { _frames = (n); _state = __LINE__; return false; case __LINE__: if ((_frames--) > 0) return false; } while (0)

// Attente jusqu’à ce qu’une condition soit vraie
#define CORO_WAIT_UNTIL(pred) \
    do { _state = __LINE__; return false; case __LINE__: if (!(pred)) return false; } while (0)

// Attente d’un std::future sans blocage (polling non bloquant)
#define CORO_AWAIT_FUTURE(fut) \
    do { _state = __LINE__; return false; case __LINE__: if ((fut).wait_for(std::chrono::seconds(0)) != std::future_status::ready) return false; } while (0)


// ─────────────────────────────────────────────
// Scheduler de coroutines
    class CoroutineManager {
    public:

        static CoroutineManager& Instance()
        {
            static CoroutineManager instance;
            return instance;
        }

        ~CoroutineManager() 
        {
            clear();
        }

        template <typename T, typename... Args>
        T& start(Args&&... args) {
            auto ptr = new T(std::forward<Args>(args)...); 
            T& ref = *ptr;
            m_routines.emplace_back(std::move(ptr));
            return ref;
        }

        void update()
        {
            update(Time::GetDeltaTime().asSeconds());
        }

        // Appeler chaque frame, avec dt en secondes
        void update(float dt) {
            std::size_t w = 0;
            for (std::size_t i = 0; i < m_routines.size(); ++i) {
                if (!m_routines[i]->tick(dt)) {       // false => encore vivante
                    if (i != w) m_routines[w] = std::move(m_routines[i]);
                    ++w;
                }
            }
            m_routines.resize(w); // supprime celles qui ont renvoyé true (terminées)
        }

        void clear() 
        {
            for (auto c : m_routines)
            {
                delete c;
            }
            m_routines.clear(); 
        }

    private:
        CoroutineManager();
        std::vector<ICoroutine*> m_routines;
    };
}