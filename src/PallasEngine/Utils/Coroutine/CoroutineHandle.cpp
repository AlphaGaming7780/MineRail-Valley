#include <PallasEngine/Utils/Coroutine/CoroutineHandle.hpp>
#include <PallasEngine/Utils/Coroutine/CoroutineScheduler.hpp>

bool pallas::CoroutineHandle::IsDone() const noexcept
{
    if (!IsValid()) return true;
    return CoroutineScheduler::Instance().IsCoroutineDone(id);
}

void pallas::CoroutineHandle::Stop() const noexcept
{
    CoroutineScheduler::Instance().StopCoroutine(id);
}

