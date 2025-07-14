#ifndef _IDELEGATE_
#define _IDELEGATE_

#include "ThreadPool/IThreadPool.h"

#include <functional>
#include <memory>
#include <mutex>
#include <vector>
#include <queue>

namespace INVENT
{
	using EventFunctionID = size_t;

	namespace ITools
	{
		std::shared_ptr<IThreadPool> GetDelegateEventThreadPool();
	}

	template<typename... Args>
	class Event 
	{
	public:
		Event() = default;
		Event(const Event&) = delete;
		void operator=(const Event&) = delete;

		using EventFunction = std::function<void(Args...)>;

		EventFunctionID AddFunction(const EventFunction& func)
		{
			if (_free_id.empty())
			{
				std::lock_guard<std::mutex> lock(_mutex_vector);
				_functions.push_back(func);
				return _functions.size() - 1;
			}
			auto id = _free_id.front();
			_functions[id] = func;
			std::lock_guard<std::mutex> lock(_mutex_queue);
			_free_id.pop();
			return id;
		}

		void RemoveFunction(const EventFunctionID& id)
		{
			if (_functions.size() > id)
			{
				_functions[id] = nullptr;
				std::lock_guard<std::mutex> lock(_mutex_queue);
				_free_id.push(id);
			}
		}

		void operator()(Args&&... args)
		{

			std::lock_guard<std::mutex> lock(_mutex_vector);

			ITools::GetDelegateEventThreadPool()->Submit(0, [...largs = std::forward<Args>(args)](std::vector<EventFunction> functions) {
				for (const auto& func : functions)
				{
					if (func)
					{
						func(largs...);
					}
				}
				}, _functions);

		}


	private:
		std::vector<EventFunction> _functions;
		std::queue<EventFunctionID> _free_id;

		std::mutex _mutex_vector;
		std::mutex _mutex_queue;

	};

}

#endif // !_IDELEGATE_

