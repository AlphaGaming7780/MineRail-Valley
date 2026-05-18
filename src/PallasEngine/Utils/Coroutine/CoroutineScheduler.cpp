#include <PallasEngine/Utils/Coroutine/CoroutineScheduler.hpp>
#include <algorithm>

namespace pallas
{
    void CoroutineScheduler::resume_next_frame(std::coroutine_handle<> h)
    {
        if (h)
            m_NextFrame.push_back(h);
    }

    void CoroutineScheduler::resume_after(std::coroutine_handle<> h, std::chrono::milliseconds ms)
    {
        if (h)
            m_Timers.emplace(TimerItem{ clk::now() + ms, h });
    }

    CoroutineHandle CoroutineScheduler::StartCoroutine(CoroutineOwner* owner, Coroutine co)
    {
        return {.id = StartCoroutineInternal(owner, std::move(co))->id };
    }

    CoroutineHandle CoroutineScheduler::StartChildCoroutine(std::coroutine_handle<> parent, Coroutine child)
    {
        RunningCoroutine* rc = FindRunning(parent);

        if (rc == nullptr)
        {
            m_Logger.Error("StartChildCoroutine : Failed to find the parent running coroutine.");
            return {.id = 0};
        }

        RunningCoroutine* childRC = StartCoroutineInternal(rc->owner, std::move(child));
        childRC->continuation = parent;

        return {.id = childRC->id};
    }


    void CoroutineScheduler::StopCoroutine(CoroutineID id)
    {
        // On ne détruit pas ici : on marque seulement pour annulation
        for (auto& rc : m_Running)
        {
            if (rc.id == id)
            {
                rc.owner = nullptr; // plus d'owner = annulée
                rc.id = 0;
            }
        }
    }

    void CoroutineScheduler::StopAllCoroutines(CoroutineOwner* owner)
    {
        for (auto& rc : m_Running)
        {
            if (rc.owner == owner)
            {
                rc.owner = nullptr;
                rc.id = 0;
            }
        }
    }

    CoroutineScheduler::RunningCoroutine* CoroutineScheduler::StartCoroutineInternal(CoroutineOwner* owner, Coroutine co)
    {
        const CoroutineID id = m_NextId++;

        m_Running.push_back(RunningCoroutine{
            .co = std::move(co),
            .owner = owner,
            .id = id
            });

        // Démarre au prochain frame
        resume_next_frame(m_Running.back().co.h);

        return &m_Running.back();
    }

    CoroutineScheduler::RunningCoroutine* CoroutineScheduler::FindRunning(std::coroutine_handle<> h)
    {
        for (auto& rc : m_Running)
        {
            if (rc.co.h == h)
                return &rc;
        }
        return nullptr;
    }

    CoroutineScheduler::RunningCoroutine* CoroutineScheduler::FindRunning(CoroutineID id)
    {
        for (auto& rc : m_Running)
        {
            if (rc.id == id)
                return &rc;
        }
        return nullptr;
    }

    bool CoroutineScheduler::IsCoroutineDone(CoroutineID id)
    {
        for (auto const& rc : m_Running)
        {
            if (rc.id == id)
                return false; // encore en cours / en attente
        }
        return true; // pas trouvé -> considérée terminée
    }


    void CoroutineScheduler::Update()
    {
#ifdef DEBUG
        PerformanceLogger::Scoped perf("CoroutineScheduler::Update");
#endif // DEBUG

        auto now = clk::now();

        // Timers prêts → on pousse les handles dans m_NextFrame
        while (!m_Timers.empty() && m_Timers.top().wake <= now)
        {
            auto h = m_Timers.top().h;
            m_Timers.pop();
            if (h)
                m_NextFrame.push_back(h);
        }

        // Exécuter les coroutines de ce frame
        std::vector<std::coroutine_handle<>> runList = std::move(m_NextFrame);
        m_NextFrame.clear();

        for (auto h : runList)
        {
            if (!h || h.done())
                continue;

            // On essaie de retrouver le RunningCoroutine associé à ce handle
            RunningCoroutine* rcPtr = FindRunning(h);

            // Si on a un RunningCoroutine associé
            if (rcPtr)
            {
                // Si annulée ou owner mort → on détruit et on ne reprend pas
                if (!rcPtr->owner || !rcPtr->owner->IsAlive())
                {
                    h.destroy();
                    rcPtr->co.h = nullptr;
                    continue;
                }

                h.resume();
            }

            // C'est possible j'ai des coroutine_handle invalide ici (on a pas trouver de runningCoroutine)
            // Car elle on été détruite par l'update lors d'une frame précédente.
            // Par exemple si la coroutine étais en waitXseconds et qu'elle a été stop durant l'attente, 
            // on enlève pas le handle de m_Timers et/ou de m_NextFrame.
            // Le pointer coroutine_handle n'est alors pas un nullptr mais un pointer qui reference du bullshit

        }

        // Nettoyage : coroutines terminées, annulées ou owners morts
        m_Running.erase(
            std::remove_if(
                m_Running.begin(), m_Running.end(),
                [&](RunningCoroutine& rc)
                {
                    const bool ownerDead = (!rc.owner || !rc.owner->IsAlive());
                    const bool noHandle = (!rc.co.h);
                    const bool done = (rc.co.h && rc.co.h.done());

                    if (ownerDead || noHandle || done)
                    {
                        if (done && rc.continuation)
                        {
                            m_NextFrame.push_back(rc.continuation);
                        }

                        if (rc.co.h)
                            rc.co.h.destroy();
                        rc.co.h = nullptr;
                        rc.owner = nullptr;
                        rc.id = 0;
                        return true;
                    }
                    return false;
                }
            ),
            m_Running.end()
        );
    }
}