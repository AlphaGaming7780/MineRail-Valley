#include <PallasEngine/Utils/Coroutine/Coroutine.hpp>
#include <PallasEngine/Utils/Coroutine/CoroutineScheduler.hpp>

namespace pallas
{
	bool Coroutine::Awaiter::await_ready() const noexcept
	{
		return !h || h.done();
	}
	void Coroutine::Awaiter::await_suspend(std::coroutine_handle<> parent)
	{
		CoroutineScheduler::Instance().StartChildCoroutine(parent, Coroutine{ h });
	}
}


