#pragma once
#include <coroutine>
#include <vector>
#include <queue>
#include <chrono>
#include <cstdint>

#include <PallasEngine/Utils/Coroutine/Coroutine.hpp>
#include <PallasEngine/Utils/Coroutine/CoroutineOwner.hpp>
#include <PallasEngine/Logging.hpp>

namespace pallas
{
    // --------------------------------------------------------
    // Scheduler des coroutines sur le thread principal
    // --------------------------------------------------------
    class CoroutineScheduler
    {
    public:
        using clk = std::chrono::steady_clock;
        using CoroutineID = std::uint64_t;

        static CoroutineScheduler& Instance()
        {
            static CoroutineScheduler inst;
            return inst;
        }

        // Reprendre la coroutine au prochain frame
        void resume_next_frame(std::coroutine_handle<> h);

        // Reprendre la coroutine après "ms"
        void resume_after(std::coroutine_handle<> h, std::chrono::milliseconds ms);

        // Appelé dans la boucle de jeu
        void Update();

        // API pour les owners
        CoroutineHandle StartCoroutine(CoroutineOwner* owner, Coroutine co);
        CoroutineHandle StartChildCoroutine(std::coroutine_handle<> parent, Coroutine child);

        void StopCoroutine(CoroutineID id);
        void StopAllCoroutines(CoroutineOwner* owner);

        bool IsCoroutineDone(CoroutineID id);

    private:
        CoroutineScheduler() = default;

        Logger m_Logger = Logger("CoroutineScheduler");

        struct TimerItem
        {
            clk::time_point wake;
            std::coroutine_handle<> h;

            bool operator<(const TimerItem& other) const
            {
                return wake > other.wake; // plus tôt = plus haut
            }
        };

        struct RunningCoroutine
        {
            Coroutine co;
            CoroutineOwner* owner = nullptr;
            std::coroutine_handle<> continuation = nullptr;
            CoroutineID id = 0;
        };

        std::priority_queue<TimerItem> m_Timers;
        std::vector<std::coroutine_handle<>> m_NextFrame;
        std::vector<RunningCoroutine> m_Running;

        CoroutineID m_NextId = 1;

        RunningCoroutine* StartCoroutineInternal(CoroutineOwner* owner, Coroutine co);

        //void SetContinuation(CoroutineID id, std::coroutine_handle<> parent);
        RunningCoroutine* FindRunning(std::coroutine_handle<> h);
        RunningCoroutine* FindRunning(CoroutineID id);
        //CoroutineOwner* FindOwner(std::coroutine_handle<> h);
    };
}