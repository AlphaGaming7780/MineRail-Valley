#pragma once
#include <coroutine>
#include <utility>

namespace pallas
{
    template<typename T>
    class Generator
    {
    public:
        struct promise_type
        {
            T current_value{};

            Generator get_return_object()
            {
                return Generator{
                    std::coroutine_handle<promise_type>::from_promise(*this)
                };
            }

            std::suspend_always initial_suspend() noexcept { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }

            std::suspend_always yield_value(T value) noexcept
            {
                current_value = std::move(value);
                return {};
            }

            void return_value(T value) noexcept
            {
                current_value = std::move(value);
            }

            void unhandled_exception() noexcept
            {
                std::terminate();
            }
        };

        using handle_t = std::coroutine_handle<promise_type>;

        Generator() = default;
        explicit Generator(handle_t h) : m_Handle(h) {}

        Generator(Generator&& other) noexcept
            : m_Handle(std::exchange(other.m_Handle, {})) {
        }

        Generator& operator=(Generator&& other) noexcept
        {
            if (this != &other)
                m_Handle = std::exchange(other.m_Handle, {});
            return *this;
        }

        Generator(const Generator&) = delete;
        Generator& operator=(const Generator&) = delete;

        ~Generator()
        {
            if (m_Handle)
                m_Handle.destroy();
        }

        bool next()
        {
            if (!m_Handle || m_Handle.done())
                return false;

            m_Handle.resume();
            return !m_Handle.done();
        }

        const T& value() const
        {
            return m_Handle.promise().current_value;
        }

        bool done() const
        {
            return !m_Handle || m_Handle.done();
        }

        // ------------------------------------------------------------
        // Range-for support
        // ------------------------------------------------------------
        struct iterator
        {
            Generator* gen = nullptr;

            iterator() = default;
            explicit iterator(Generator* g) : gen(g)
            {
                if (gen && !gen->next())
                    gen = nullptr;
            }

            iterator& operator++()
            {
                if (!gen->next())
                    gen = nullptr;
                return *this;
            }

            const T& operator*() const
            {
                return gen->value();
            }

            bool operator!=(const iterator& other) const
            {
                return gen != other.gen;
            }
        };

        iterator begin() { return iterator(this); }
        iterator end() { return iterator(nullptr); }

    private:
        handle_t m_Handle{};
    };
}