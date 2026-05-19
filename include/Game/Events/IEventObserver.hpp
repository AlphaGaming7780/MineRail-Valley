#pragma once

namespace Game
{

	class IEventObserverBase
	{
	public:
		virtual ~IEventObserverBase() = default;
	};

	template<typename T>
	class IEventObserver : IEventObserverBase
	{
	public:
		virtual ~IEventObserver() = default;
		virtual void OnEvent(const T& event) = 0;
	};
}