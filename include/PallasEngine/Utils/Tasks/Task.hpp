#pragma once
#include <future>
#include <optional>
#include <PallasEngine/Utils/Tasks/ThreadPool.hpp>
#include <PallasEngine/Utils/Tasks/Cancellation.hpp>
#include <PallasEngine/Utils/Coroutine.h>

namespace pallas
{
    // ============================================================
    //          Task<T> - VERSION GENERIQUE (T != void)
    // ============================================================
    template<typename T>
    class TaskT
    {
    public:
        TaskT() = default;

        // Exécution en background
        template<typename F>
        TaskT(F&& fn)
        {
            fut = ThreadPool::Instance().submit(std::forward<F>(fn));
        }

        // Version avec token
        template<typename F>
        TaskT(F&& fn, CancellationToken token)
        {
            fut = ThreadPool::Instance().submit(fn = std::forward<F>(fn), token->T{
                if (token.IsCancellationRequested())
                    throw std::runtime_error("Task cancelled");
                return fn();
                });
        }

        bool IsDone() const
        {
            using namespace std::chrono_literals;
            return fut.valid() && fut.wait_for(0ms) == std::future_status::ready;
        }

        T GetResult()
        {
            return fut.get();
        }

        std::optional<T> TryGetResult()
        {
            if (!IsDone()) return std::nullopt;
            return fut.get();
        }

        // Awaiter pour coroutine
        struct Awaiter
        {
            TaskT& task;

            bool await_ready() const { return task.IsDone(); }

            void await_suspend(std::coroutine_handle<> h)
            {
                pallas::CoroutineScheduler::instance().resume_next_frame(h);
            }

            T await_resume()
            {
                return task.GetResult();
            }
        };

        Awaiter GetAwaiter() {
            return Awaiter{ *this };
        }

        auto operator co_await() { return Awaiter{ *this }; }

    private:
        std::future<T> fut;
    };


    // ============================================================
    //                 SPEC POUR VOID (Task)
    // ============================================================
    template<>
    class TaskT<void>
    {
    public:
        TaskT() = default;

        template<typename F>
        TaskT(F&& fn)
        {
            fut = ThreadPool::Instance().submit(std::forward<F>(fn));
        }

        template<typename F>
        TaskT(F&& fn, CancellationToken token)
        {
            fut = ThreadPool::Instance().submit(fn = std::forward<F>(fn), token{
                if (token.IsCancellationRequested())
                    throw std::runtime_error("Task cancelled");
                fn();
                });
        }

        bool IsDone() const
        {
            using namespace std::chrono_literals;
            return fut.valid() && fut.wait_for(0ms) == std::future_status::ready;
        }

        void Wait()
        {
            fut.get();
        }

        std::optional<bool> TryWait()
        {
            if (!IsDone()) return std::nullopt;
            fut.get();
            return true;
        }

        struct Awaiter
        {
            TaskT<void>& task;

            bool await_ready() const { return task.IsDone(); }

            void await_suspend(std::coroutine_handle<> h)
            {
                extern void Pallas_ScheduleNextFrame(std::coroutine_handle<>);
                Pallas_ScheduleNextFrame(h);
            }

            void await_resume()
            {
                task.Wait();
            }
        };

        auto operator co_await() { return Awaiter{ *this }; }

    private:
        std::future<void> fut;
    };


    // ============================================================
    // Alias pour avoir "Task" comme en C#
    // ============================================================
    using Task = TaskT<void>;


    // ============================================================
    // TaskCompletionSource<T>
    // ============================================================
    template<typename T>
    class TaskCompletionSource
    {
    public:
        TaskT<T> GetTask()
        {
            return TaskT<T>(future);
        }

        void SetResult(const T& value)
        {
            promise.set_value(value);
            future = promise.get_future();
        }

        void SetException(std::exception_ptr e)
        {
            promise.set_exception(e);
            future = promise.get_future();
        }

        void Cancel()
        {
            promise.set_exception(std::make_exception_ptr(
                std::runtime_error("Task cancelled")));
            future = promise.get_future();
        }

    private:
        std::promise<T> promise;
        std::future<T> future = promise.get_future();
    };


    // Version TaskCompletionSource<void>
    template<>
    class TaskCompletionSource<void>
    {
    public:
        Task GetTask()
        {
            return Task(future);
        }

        void SetResult()
        {
            promise.set_value();
            future = promise.get_future();
        }

        void SetException(std::exception_ptr e)
        {
            promise.set_exception(e);
            future = promise.get_future();
        }

        void Cancel()
        {
            promise.set_exception(std::make_exception_ptr(
                std::runtime_error("Task cancelled")));
            future = promise.get_future();
        }

    private:
        std::promise<void> promise;
        std::future<void> future = promise.get_future();
    };

}