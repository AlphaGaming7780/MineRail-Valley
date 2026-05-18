#include <PallasEngine/Utils/Coroutine/CoroutineOwner.hpp>
#include <PallasEngine/Utils/Coroutine/Coroutine.hpp>
#include <PallasEngine/Utils/Coroutine/CoroutineScheduler.hpp>

namespace pallas
{
    CoroutineOwner::CoroutineOwner()
        : m_Alive(true)
    {
    }

    CoroutineOwner::~CoroutineOwner()
    {
        m_Alive = false;
        StopAllCoroutines();
    }

    CoroutineHandle CoroutineOwner::StartCoroutine(Coroutine co)
    {
        return CoroutineScheduler::Instance().StartCoroutine(this, std::move(co));
    }

    void CoroutineOwner::StopCoroutine(std::uint64_t id)
    {
        CoroutineScheduler::Instance().StopCoroutine(id);
    }

    void CoroutineOwner::StopAllCoroutines()
    {
        CoroutineScheduler::Instance().StopAllCoroutines(this);
    }
}