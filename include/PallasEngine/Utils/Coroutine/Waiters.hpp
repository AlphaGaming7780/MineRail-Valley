#pragma once
#include <PallasEngine/Utils/Coroutine.hpp>
#include <PallasEngine/Utils/Coroutine/CoroutineScheduler.hpp>
#include <functional>

namespace pallas
{
    struct Waiters 
    {
        // --------------------------------------------------------
        // Attendre le prochain frame
        // --------------------------------------------------------
        struct NextFrame
        {
            bool await_ready() const noexcept { return false; }

            void await_suspend(std::coroutine_handle<> h) const
            {
                CoroutineScheduler::Instance().resume_next_frame(h);
            }

            void await_resume() const noexcept {}
        };


        // --------------------------------------------------------
        // Attendre X secondes
        // --------------------------------------------------------
        struct WaitForSeconds
        {
            double sec;

            bool await_ready() const noexcept { return sec <= 0.0; }

            void await_suspend(std::coroutine_handle<> h) const
            {
                using namespace std::chrono;
                CoroutineScheduler::Instance().resume_after(
                    h,
                    duration_cast<milliseconds>(duration<double>(sec))
                );
            }

            void await_resume() const noexcept {}
        };


        // --------------------------------------------------------
        // Attendre une condition (predicate)
        // --------------------------------------------------------
        struct WaitUntil
        {
            std::function<bool()> predicate;

            bool await_ready() const noexcept { return predicate(); }

            void await_suspend(std::coroutine_handle<> h) const
            {
                // On re-check chaque frame
                CoroutineScheduler::Instance().resume_next_frame(h);
            }

            void await_resume() const noexcept {}
        };

        // --------------------------------------------------------
        // Attendre la fin d'une autre coroutine
        // --------------------------------------------------------
        struct Join
        {
            Coroutine child;

            explicit Join(Coroutine&& c)
                : child(std::move(c))
            {
            }

            bool await_ready() const noexcept
            {
                return !child.h || child.h.done();
            }

            void await_suspend(std::coroutine_handle<> parent)
            {
                CoroutineScheduler::Instance().StartChildCoroutine(parent, std::move(child));
            }

            void await_resume() const noexcept {}
        };
    };
}