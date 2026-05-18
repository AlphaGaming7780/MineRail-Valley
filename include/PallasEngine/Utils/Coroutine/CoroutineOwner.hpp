#pragma once
#include <cstdint>
#include <PallasEngine/Utils/Coroutine/Coroutine.hpp>
#include <PallasEngine/Utils/Coroutine/CoroutineHandle.hpp>

namespace pallas
{
    class CoroutineScheduler;

    class CoroutineOwner
    {
    public:
        CoroutineOwner();
        ~CoroutineOwner();

        bool IsAlive() const { return m_Alive; }

        // Lance une coroutine attachée à cet owner
        CoroutineHandle StartCoroutine(Coroutine co);

        // Stop une coroutine par ID
        void StopCoroutine(std::uint64_t id);

        // Stop toutes les coroutines attachées à cet owner
        void StopAllCoroutines();

    private:
        bool m_Alive = true;

        friend class CoroutineScheduler;
    };
}