#include "IEpch.h"
#include "IEngine.h"

#include "IBase/IWindow.h"

static const auto StartTimePoint = std::chrono::high_resolution_clock::now();

namespace INVENT
{
	IEngine::IEngine()
		: _iwindow(nullptr)
	{
	}

	std::shared_ptr<IEngine> IEngine::InstancePtr()
	{
		static std::shared_ptr<IEngine> iengine(new IEngine);
		return iengine;
	}

	IWindow* IEngine::GetIWindow()
	{
		return _iwindow;
	}

	unsigned int IEngine::GetWindowSizeX()
	{
		return _iwindow->GetWidth();
	}

	unsigned int IEngine::GetWindowSizeY()
	{
		return _iwindow->GetHeight();
	}

	float IEngine::GetEngineMilliseconds()
	{
		return std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now() - StartTimePoint).count();
	}

	const std::chrono::steady_clock::time_point& IEngine::GetEngineStartTimePoint()
	{
		return StartTimePoint;
	}

	void IEngine::SetIWindow(IWindow* window)
	{
		_iwindow = window;
	}

	static std::queue<size_t> NormalInputFunctionIDs;
	static std::queue<size_t> CursorPositionFunctionIDs;
	IEngine::NormalInputFunctionID IEngine::RegisterNormalProcessInputFunction(std::function<void(float)>&& callback, int key)
	{
		size_t id = _normal_input_callbacks.size();
		if (!NormalInputFunctionIDs.empty())
		{
			id = NormalInputFunctionIDs.front();
			NormalInputFunctionIDs.pop();
			_normal_input_callbacks[id].first = std::move(callback);
		}
		else
		{
			_normal_input_callbacks.emplace_back(std::move(callback), key);
		}
		return id;
	}

	void IEngine::CancellationNormalProcessInputFunction(NormalInputFunctionID id)
	{
		if (id >= _normal_input_callbacks.size())
		{
			return;
		}
		_normal_input_callbacks[id].first = nullptr;
		NormalInputFunctionIDs.emplace(id);
	}

	IEngine::CursorPositionFunctionID IEngine::RegisterCursorPositionFunction(std::function<void(float, bool, double, double)>&& callback)
	{
		size_t id = IEngine::_cursor_position_input_callbacks.size();
		if (!CursorPositionFunctionIDs.empty())
		{
			id = CursorPositionFunctionIDs.front();
			CursorPositionFunctionIDs.pop();
			IEngine::_cursor_position_input_callbacks[id] = std::move(callback);
		}
		else
		{
			IEngine::_cursor_position_input_callbacks.emplace_back(std::move(callback));
		}
		return id;
	}

	void IEngine::CancellationCursorPositionFunction(CursorPositionFunctionID id)
	{
		if (id >= IEngine::_cursor_position_input_callbacks.size())
		{
			return;
		}
		IEngine::_cursor_position_input_callbacks[id] = nullptr;
		CursorPositionFunctionIDs.emplace(id);
	}
}