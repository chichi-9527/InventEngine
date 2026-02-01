#include "IEpch.h"
#include "IScene.h"

#include "IEngine.h"

namespace INVENT
{
	IScene::IScene()
	{}

	IScene::~IScene()
	{}

	std::shared_ptr<IScene> IScene::InstancePtr()
	{
		static auto scene = std::shared_ptr<IScene>(new IScene);
		return scene;
	}

	IThreadPool* IScene::_get_engine_work_thread_pool()
	{
		return IEngine::InstancePtr()->GetWorkThreadPool();
	}
}
