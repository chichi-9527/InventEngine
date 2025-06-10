#ifndef _BINDEVENT_
#define _BINDEVENT_

#include <functional>
#include <mutex>

#include "ILog.h"

namespace INVENT
{
	template<typename... Args>
	class EventBase {
	public:
		using EventFunction = std::function<void(Args...)>;

		// 添加事件处理函数
		void addFunction(EventFunction func)
		{
			std::lock_guard<std::mutex> lock(mtx);
			functions.push_back(std::move(func));
		}

		// 移除事件处理函数
		void removeFunction(const EventFunction& func)
		{
			std::lock_guard<std::mutex> lock(mtx);
			functions.erase(std::remove(functions.begin(), functions.end(), func), functions.end());
		}

		// 重载 () 运算符来触发事件
		void operator()(Args... args)
		{
			std::vector<EventFunction> copy;
			{
				std::lock_guard<std::mutex> lock(mtx);
				copy = functions;
			}

			for (const auto& func : copy)
			{
				try
				{
					func(args...);
				}
				catch (const std::exception& e)
				{
					ILog::Instance().IERROR("Exception in event handler: ");
					ILog::Instance().IERROR(e.what());
				}
			}
		}

	private:
		std::vector<EventFunction> functions;
		std::mutex mtx;
	};
}



#define IEVENT(event_name, ...) \
class event_name : public EventBase<__VA_ARGS__> {};\
private:\
event_name EVENT_##event_name;\
public:\
event_name& get_##event_name(){\
	return EVENT_##event_name;}

// 定义用于指针的事件绑定宏
#define IBIND_EVENT_P(target, event_name, class_name, func) \
    target->get_##event_name().addFunction([this](auto&&... args) { \
        (this->*(&class_name::func))(std::forward<decltype(args)>(args)...); \
    });

// 定义用于对象的事件绑定宏
#define IBIND_EVENT(target, event_name, class_name, func) \
    target.get_##event_name().addFunction([this](auto&&... args) { \
        (this->*(&class_name::func))(std::forward<decltype(args)>(args)...); \
    });

// 定义用于指针的事件移除宏
#define IREMOVE_EVENT_P(target, event_name, class_name, func) \
    auto removeFunc##event_name = [this](auto&&... args) { \
        (this->*(&class_name::func))(std::forward<decltype(args)>(args)...); \
    }; \
    target->get_##event_name().removeFunction(removeFunc##event_name);

// 定义用于对象的事件移除宏
#define IREMOVE_EVENT(target, event_name, class_name, func) \
    auto removeFunc##event_name = [this](auto&&... args) { \
        (this->*(&class_name::func))(std::forward<decltype(args)>(args)...); \
    }; \
    target.get_##event_name().removeFunction(removeFunc##event_name);



#endif // !_BINDEVENT_
