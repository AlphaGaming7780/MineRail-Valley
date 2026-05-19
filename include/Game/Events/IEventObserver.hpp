
namespace Game
{
	template<typename T>
	class IEventObserver
	{
	public:
		virtual ~IEventObserver() = default;
		virtual void OnEvent(const T& event) = 0;
	};
}


