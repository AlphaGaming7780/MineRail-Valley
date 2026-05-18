#pragma once

#include <vector>
#include <algorithm>

#include <PallasEngine/Utils/Delegate.hpp>
#include <PallasEngine/Utils/Coroutine/Generator.hpp>

namespace pallas
{
    template<typename... Args>
    class Event
    {
    public:
        using Handler = Delegate<Args...>;

        // ------------------------------------------------------------
        // Subscribe / Unsubscribe
        // ------------------------------------------------------------
        void subscribe(const Handler& h)
        {
            m_Handlers.push_back(h);
        }

        void unsubscribe(const Handler& h)
        {
            m_Handlers.erase(
                std::remove(m_Handlers.begin(), m_Handlers.end(), h),
                m_Handlers.end()
            );
        }

        // ------------------------------------------------------------
        // Operator += / -=
        // ------------------------------------------------------------
        Event& operator+=(const Handler& h)
        {
            subscribe(h);
            return *this;
        }

        Event& operator-=(const Handler& h)
        {
            unsubscribe(h);
            return *this;
        }

        // Cast automatique pour les fonctions libres
        Event& operator+=(void(*func)(Args...))
        {
            subscribe(Handler(func));
            return *this;
        }

        Event& operator-=(void(*func)(Args...))
        {
            unsubscribe(Handler(func));
            return *this;
        }

        // ------------------------------------------------------------
        // Invocation
        // ------------------------------------------------------------
        void operator()(Args... args)
        {
            for (auto& h : m_Handlers)
                h(args...);
        }

        // ------------------------------------------------------------
        // Generator iterator
        // ------------------------------------------------------------
        pallas::Generator<Handler&> iterate()
        {
            for (auto& h : m_Handlers)
                co_yield h;
        }

        // ------------------------------------------------------------
        // Range-for support (optional)
        // ------------------------------------------------------------
        auto begin() { return m_Handlers.begin(); }
        auto end() { return m_Handlers.end(); }

        // ------------------------------------------------------------
        // Utils
        // ------------------------------------------------------------
        void clear()
        {
            m_Handlers.clear();
        }

        std::size_t count() const noexcept
        {
            return m_Handlers.size();
        }

        bool empty() const noexcept
        {
            return m_Handlers.empty();
        }

        void reserve(std::size_t n)
        {
            m_Handlers.reserve(n);
        }

    private:
        std::vector<Handler> m_Handlers;
    };

}