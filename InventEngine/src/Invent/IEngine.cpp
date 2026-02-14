#include "IEpch.h"
#include "IEngine.h"

#include "ThreadPool/IThreadPool.h"
#include "IBase/IWindow.h"
#include "IBase/IBaseLevel.h"
#include "IBase/IGameInstance.h"
#include "IBase/IScene.h"

static const auto StartTimePoint = std::chrono::high_resolution_clock::now();

namespace INVENT
{


	IEngine::IEngine()
		: _threadpool(nullptr)
		, _work_thread_pool(new IThreadPool)
	{
		_main_scene = IScene::CreateInstancePtr();
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

	std::shared_ptr<IScene> IEngine::GetMainScene()
	{
		return _main_scene;
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

	std::shared_ptr<IRenderThread> IEngine::GetRenderThreadPtr()
	{
		return _iwindow_ptr->_render_thread;
	}

}