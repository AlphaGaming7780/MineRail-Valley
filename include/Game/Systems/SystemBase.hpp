#pragma once

#include <PallasEngine/Logging/Logger.hpp>

//#include <Game/World.h>

namespace Game
{
    class World;

    class SystemBase // : IEventObserver<GameEvent>, IEventObserver<...> ...
    {
    public:
        SystemBase() = default;

        explicit SystemBase(World* world, pallas::Logger& logger)
            : m_World(world), m_Logger(logger)
        {
        }

        virtual ~SystemBase() = default;

        virtual void OnCreate() {};
        virtual void Update() = 0;
        virtual void OnDestroy() {};

        void SetEnable(bool enable);
        bool IsEnabled() const;
        virtual void OnEnabled(bool enabled) {};

    protected:
        World* m_World;
        pallas::Logger& m_Logger;

        // Bind c'est func a des event sur l'event manager ?
        virtual void OnGameLoadingStart(GameMode mode, Purpose purpose) {};
        virtual void OnGameLoadingComplete(GameMode mode, Purpose purpose) {};

    private:
        bool m_Enabled = true;

    };
}