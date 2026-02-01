#include "IEpch.h"
#include "IEngine.h"

#include "ThreadPool/IThreadPool.h"
#include "IBase/IWindow.h"
#include "IBase/IBaseLevel.h"
#include "IBase/IGameInstance.h"

static const auto StartTimePoint = std::chrono::high_resolution_clock::now();

namespace INVENT
{


	IEngine::IEngine()
		: _threadpool(nullptr)
		, _work_thread_pool(new IThreadPool)
	{
		_work_thread_pool->Start();
	}

	IEngine::~IEngine()
	{
		ShutdownThreadPool();
		if (_threadpool)
		{
			delete _threadpool;
			_threadpool = nullptr;
		}
		if (_work_thread_pool)
		{
			_work_thread_pool->Shutdown();
			delete _work_thread_pool;
			_work_thread_pool = nullptr;
		}
		
	}

	std::shared_ptr<IEngine> IEngine::InstancePtr()
	{
		static std::shared_ptr<IEngine> iengine(new IEngine);
		return iengine;
	}

	void IEngine::Start()
	{
		if (_iwindow_ptr)
		{
			_iwindow_ptr->Start();
		}
		else
		{
			INVENT_LOG_WARNING("has no window, you need create game window before start");
		}
	}

	std::shared_ptr<IWindow> IEngine::GetIWindow()
	{
		return _iwindow_ptr;
	}

	unsigned int IEngine::GetWindowSizeX()
	{
		return _iwindow_ptr->GetWidth();
	}

	unsigned int IEngine::GetWindowSizeY()
	{
		return _iwindow_ptr->GetHeight();
	}

	void IEngine::SetGameInstance(std::shared_ptr<IBaseGameInstance> game_instance_ptr)
	{
		_game_instance_ptr = game_instance_ptr;
	}

	float IEngine::GetEngineMilliseconds()
	{
		return std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now() - StartTimePoint).count();
	}

	const std::chrono::steady_clock::time_point& IEngine::GetEngineStartTimePoint()
	{
		return StartTimePoint;
	}

	void IEngine::SetThreadPoolThreadNumAndPriorityNum(unsigned int t_num, unsigned int p_num)
	{
		if (!_threadpool)
		{
			_threadpool = new IThreadPool(t_num, p_num);
			return;
		}
		_threadpool->SetThreadPriorityNum(t_num, p_num);
	}

	void IEngine::StartThreadPool()
	{
		if (!_threadpool)
		{
			_threadpool = new IThreadPool();
		}
		_threadpool->Start();
	}

	void IEngine::ShutdownThreadPool()
	{
		if (_threadpool)
			_threadpool->Shutdown();
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