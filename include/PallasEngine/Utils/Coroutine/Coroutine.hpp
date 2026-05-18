#pragma once
#include <coroutine>
#include <vector>
#include <queue>
#include <chrono>
#include <utility>

namespace pallas
{
    // --------------------------------------------------------
    // Coroutine main-thread
    // --------------------------------------------------------
    struct Coroutine
    {
        struct promise_type
        {
            std::coroutine_handle<> continuation = nullptr;

            Coroutine get_return_object()
            {
                return Coroutine{
                    std::coroutine_handle<promise_type>::from_promise(*this)
                };
            }

            std::suspend_always initial_suspend() noexcept { return {}; }

            auto final_suspend() noexcept
            {
                struct Awaiter
                {
                    std::coroutine_handle<> continuation;

                    bool await_ready() const noexcept { return false; }

                    void await_suspend(std::coroutine_handle<>) const noexcept
                    {
                        if (continuation)
                            continuation.resume();
                    }

                    void await_resume() const noexcept {}
                };

                return Awaiter{ continuation };
            }

            void return_void() noexcept {}
            void unhandled_exception() noexcept { std::terminate(); }

            void set_continuation(std::coroutine_handle<> h) noexcept
            {
                continuation = h;
            }
        };

        using handle_t = std::coroutine_handle<promise_type>;
        handle_t h{};

        Coroutine() = default;
        explicit Coroutine(handle_t handle) : h(handle) {}

        Coroutine(Coroutine&& other) noexcept
            : h(std::exchange(other.h, {})) {
        }

        Coroutine& operator=(Coroutine&& other) noexcept
        {
            if (this != &other)
                h = std::exchange(other.h, {});
            return *this;
        }

        Coroutine(const Coroutine&) = delete;
        Coroutine& operator=(const Coroutine&) = delete;

        ~Coroutine() = default;

        // --------------------------------------------------------
        // Awaiter commun
        // --------------------------------------------------------
        struct Awaiter
        {
            handle_t h;

            bool await_ready() const noexcept;
            void await_suspend(std::coroutine_handle<> parent);
            void await_resume() const noexcept {}
        };

        // --------------------------------------------------------
        // Rendre Coroutine awaitable
        // --------------------------------------------------------
        auto operator co_await() & noexcept
        {
            return Awaiter{ h };
        }

        auto operator co_await() && noexcept
        {
            return Awaiter{ std::exchange(h, {}) };
        }
    };
}