#include <IEpch.h>
#include "IEngineTools.h"

#include "ThreadPool/IThreadPool.h"
#include "IMemPool/IMemPool.h"

#include <filesystem>

static auto RunPath = std::filesystem::current_path().string();

namespace INVENT
{
	IEngineTools::IEngineTools()
	{
		
	}
	IEngineTools::~IEngineTools()
	{}

	const std::string& IEngineTools::GetRunPath()
	{
		return RunPath;
	}

	std::vector<char> IEngineTools::ReadFile(const std::string& path)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);
		if (!file.is_open())
		{
			INVENT_LOG_ERROR(std::format("ERROR : [ ShaderCompiler ] failed to open file : {} \n", path));
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buf(fileSize + 1);
		file.seekg(0);
		file.read(buf.data(), fileSize);
		file.close();
		buf[fileSize] = '\0';
		return buf;
	}

	IEngineTools& IEngineTools::Instance()
	{
		static IEngineTools et;
		return et;
	}

	void IEngineTools::Init()
	{
		_init_threadpools();
	}
	void IEngineTools::Clear()
	{
		_clear_threadpools();
	}

	void IEngineTools::_init_threadpools()
	{
		_work_thread_pool = new IThreadPool(2, 1);
		_work_thread_pool->Start();
	}

	void IEngineTools::_clear_threadpools()
	{
		if (_work_thread_pool)
		{
			_work_thread_pool->Shutdown();
			delete _work_thread_pool;
			_work_thread_pool = nullptr;
		}
	}

	void IEngineTools::_init_mem_pools()
	{
		_gobal_memory_pool = IMemPool::CreatePool();
	}

	void IEngineTools::_clear_mem_pools()
	{
		IMemPool::DestroyPool(_gobal_memory_pool);
	}

	


}