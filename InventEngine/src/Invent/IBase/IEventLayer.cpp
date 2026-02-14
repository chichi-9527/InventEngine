#include "IEpch.h"
#include "IEventLayer.h"

namespace INVENT
{
	IEventLayer::IEventLayer()
	{}

	IEventLayer::~IEventLayer()
	{}

	IEventLayer::EventKeyFrameFunctionID IEventLayer::RegisterEventKeyFrameFunction(std::function<void(float)>&& callback, int key)
	{
		std::lock_guard<std::mutex> lock(_mutex);

		size_t id = _normal_input_callbacks.size();
		if (!NormalInputFunctionIDs.empty())
		{
			id = NormalInputFunctionIDs.front();
			NormalInputFunctionIDs.pop();
			_normal_input_callbacks[id] = { std::move(callback), key };
		}
		else
		{
			_normal_input_callbacks.emplace_back(std::move(callback), key);
		}
		return id;
	}

	void IEventLayer::CancellationEventKeyFrameFunction(EventKeyFrameFunctionID id)
	{
		std::lock_guard<std::mutex> lock(_mutex);

		if (id < _normal_input_callbacks.size())
		{
			_normal_input_callbacks[id].first = nullptr;
			NormalInputFunctionIDs.emplace(id);
		}
		
	}

	IEventLayer::CursorPositionFunctionID IEventLayer::RegisterCursorPositionFunction(std::function<void(float, bool, double, double)>&& callback)
	{
		std::lock_guard<std::mutex> lock(_mutex);

		size_t id = _cursor_position_input_callbacks.size();
		if (!CursorPositionFunctionIDs.empty())
		{
			id = CursorPositionFunctionIDs.front();
			CursorPositionFunctionIDs.pop();
			_cursor_position_input_callbacks[id] = std::move(callback);
		}
		else
		{
			_cursor_position_input_callbacks.emplace_back(std::move(callback));
		}
		return id;
	}

	void IEventLayer::CancellationCursorPositionFunction(CursorPositionFunctionID id)
	{
		std::lock_guard<std::mutex> lock(_mutex);

		if (id < _cursor_position_input_callbacks.size())
		{
			_cursor_position_input_callbacks[id] = nullptr;
			CursorPositionFunctionIDs.emplace(id);
		}
		
	}

	IEventLayer::EventKeyFunctionID IEventLayer::RegisterEventKeyFunction(std::function<void()>&& callback, int key, int action, int mods)
	{
		std::lock_guard<std::mutex> lock(_mutex);

		size_t id = _event_key_callbacks.size();
		if (!EventKeyFunctionIDs.empty())
		{
			id = EventKeyFunctionIDs.front();
			EventKeyFunctionIDs.pop();
			_event_key_callbacks[id] = { std::move(callback), key, action, mods };
		}
		else
		{
			IEventLayer::_event_key_callbacks.emplace_back(std::move(callback), key, action, mods);
		}
		return id;
	}

	void IEventLayer::CancellationEventKeyFunction(EventKeyFunctionID id)
	{
		std::lock_guard<std::mutex> lock(_mutex);

		if (id < _event_key_callbacks.size())
		{
			_event_key_callbacks[id] = { nullptr, 0, 0, 0 };
			EventKeyFunctionIDs.emplace(id);
		}
		
	}

	IEventLayer::EventMouseButtonFunctionID IEventLayer::RegisterEventMouseButtonFunction(std::function<void(double, double)>&& callback, int button, int action, int mods)
	{
		std::lock_guard<std::mutex> lock(_mutex);

		size_t id = _event_mouse_button_callbacks.size();
		if (!EventMouseButtonFunctionIDs.empty())
		{
			id = EventMouseButtonFunctionIDs.front();
			EventMouseButtonFunctionIDs.pop();
			_event_mouse_button_callbacks[id] = { std::move(callback), button, action, mods };
		}
		else
		{
			IEventLayer::_event_mouse_button_callbacks.emplace_back(std::move(callback), button, action, mods);
		}
		return id;
	}

	void IEventLayer::CancellationEventMouseButtonFunction(EventMouseButtonFunctionID id)
	{
		std::lock_guard<std::mutex> lock(_mutex);

		if (id < _event_mouse_button_callbacks.size())
		{
			_event_mouse_button_callbacks[id] = { nullptr, 0, 0, 0 };
			EventMouseButtonFunctionIDs.emplace(id);
		}
	}

	IEventLayer::EventScrollFunctionID IEventLayer::RegisterEventScrollFunction(std::function<void(double, double, double, double)>&& callback)
	{
		std::lock_guard<std::mutex> lock(_mutex);

		size_t id = _event_scroll_callbacks.size();
		if (!EventScrollFunctionIDs.empty())
		{
			id = EventScrollFunctionIDs.front();
			EventScrollFunctionIDs.pop();
			_event_scroll_callbacks[id] = std::move(callback);
		}
		else
		{
			IEventLayer::_event_scroll_callbacks.emplace_back(std::move(callback));
		}
		return id;
	}

	void IEventLayer::CancellationEventScrollFunction(EventScrollFunctionID id)
	{
		std::lock_guard<std::mutex> lock(_mutex);

		if (id < _event_scroll_callbacks.size())
		{
			_event_scroll_callbacks[id] = nullptr;
			EventScrollFunctionIDs.emplace(id);
		}
	}

	IEventLayer::EventCursorPositionFunctionID IEventLayer::RegisterEventCursorPositionFunction(std::function<void(double, double)>&& callback)
	{
		std::lock_guard<std::mutex> lock(_mutex);

		size_t id = _event_cursor_position_callbacks.size();
		if (!EventCursorPositionFunctionIDs.empty())
		{
			id = EventCursorPositionFunctionIDs.front();
			EventCursorPositionFunctionIDs.pop();
			_event_cursor_position_callbacks[id] = std::move(callback);
		}
		else
		{
			IEventLayer::_event_cursor_position_callbacks.emplace_back(std::move(callback));
		}
		return id;
	}

	void IEventLayer::CancellationEventCursorPositionFunction(EventCursorPositionFunctionID id)
	{
		std::lock_guard<std::mutex> lock(_mutex);

		if (id < _event_cursor_position_callbacks.size())
		{
			_event_cursor_position_callbacks[id] = nullptr;
			EventCursorPositionFunctionIDs.emplace(id);
		}
	}

	IEventLayer::IsEventDone IEventLayer::EVENT_CURSOR_POSITION_FRAME(float delta, bool cursor_inside_window, double xpos, double ypos)
	{
		for (auto& func : _cursor_position_input_callbacks)
		{
			func(delta, cursor_inside_window, xpos, ypos);
		}
		return EventCursorPositionFrameDone;
	}

	IEventLayer::IsEventDone IEventLayer::EVENT_KEY_FRAME(float delta, GLFWwindow* glfw_window)
	{
		for (auto& [func, key] : _normal_input_callbacks)
		{
			if (func && glfwGetKey(glfw_window, key) == (GLFW_PRESS | GLFW_RELEASE))
			{
				func(delta);
			}
		}
		return EventKeyFrameDone;
	}

	IEventLayer::IsEventDone IEventLayer::EVENT_KEY(int key, int action, int mods)
	{
		for (auto& [func, Key, Action, Mods] : _event_key_callbacks)
		{
			//if (func && Key == key && Action == action && (!Mods || Mods == mods))
			if (func && Key == key && Action == action && (Mods == (Mods & mods)))
			{
				func();
			}
		}
		return EventKeyDone;
	}

	IEventLayer::IsEventDone IEventLayer::EVENT_MOUSE_BUTTON(double xpos, double ypos, int button, int action, int mods)
	{
		for (auto& [func, Button, Action, Mods] : _event_mouse_button_callbacks)
		{
			//if (func && Button == button && Action == action && (!Mods || Mods == mods))
			if (func && Button == button && Action == action && (Mods == (Mods & mods)))
			{
				func(xpos, ypos);
			}
		}
		return EventMouseButtonDone;
	}

	IEventLayer::IsEventDone IEventLayer::EVNET_SCROLL(double cursor_xpos, double cursor_ypos, double xoffset, double yoffset)
	{
		for (auto& func : _event_scroll_callbacks)
		{
			if (func)
			{
				func(cursor_xpos, cursor_ypos, xoffset, yoffset);
			}
		}
		return EventScrollDone;
	}

	IEventLayer::IsEventDone IEventLayer::EVENT_CURSOR_POSITION(double xpos, double ypos)
	{
		for (auto& func : _event_cursor_position_callbacks)
		{
			if (func)
			{
				func(xpos, ypos);
			}
		}
		return EventCursorPositionDone;
	}


}


  