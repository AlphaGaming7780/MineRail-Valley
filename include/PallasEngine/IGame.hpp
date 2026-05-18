#pragma once

namespace pallas
{
	__interface IGame
	{
    public:
        virtual void onStart() = 0;
        virtual void onUpdate() = 0;
        virtual void onShutdown() = 0;
	};
}