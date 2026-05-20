#pragma once

#include <PallasEngine/Logging/Logger.hpp>
#include <Game/GameStates.h>

#include <Game/Events.h>

namespace Game
{
    class World;

    class SystemBase : public IEventObserver<LoadingStart>, public IEventObserver<LoadingComplete>
    {
    public:
        //SystemBase() = default;

        explicit SystemBase(World* world, pallas::Logger& logger)
            : m_World(world), m_Logger(logger)
        {
            EventManager& em = EventManager::Instance();
            em.Register<LoadingStart>(this);
            em.Register<LoadingComplete>(this);
        }

        virtual ~SystemBase()
        {
            EventManager& em = EventManager::Instance();
            em.UnRegister<LoadingStart>(this);
            em.UnRegister<LoadingComplete>(this);
        }

        virtual void OnCreate() {};
        virtual void Update() = 0;
        virtual void OnDestroy() {};

        void SetEnable(bool enable);
        bool IsEnabled() const;
        virtual void OnEnabled(bool enabled) {};

        void OnEvent(const LoadingStart& ev);
        void OnEvent(const LoadingComplete& ev);

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