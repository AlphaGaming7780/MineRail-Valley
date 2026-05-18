#pragma once
#include <cstdint>

namespace pallas
{
    class CoroutineScheduler;

    struct CoroutineHandle
    {
        std::uint64_t id = 0;

        bool IsValid() const noexcept { return id != 0; }
        bool IsDone() const noexcept;
        void Stop() const noexcept;
    };
}