#ifndef _IENGINETOOLS_
#define _IENGINETOOLS_

#include <memory>
#include <string>

namespace INVENT
{
	class IThreadPool;
	class IMemPool;

	class IEngineTools
	{
		IEngineTools();
	public:
		~IEngineTools();

		static const std::string& GetRunPath();
		static std::vector<char> ReadFile(const std::string& path);

		static IEngineTools& Instance();
		

		IThreadPool* GetWorkThreadPool() const { return _work_thread_pool; }
		IMemPool* GetMemPoolPool() const { return _gobal_memory_pool; }

		void Init();
		void Clear();

	private:
		void _init_threadpools();
		void _clear_threadpools();
		void _init_mem_pools();
		void _clear_mem_pools();

	private:
		IThreadPool* _work_thread_pool = nullptr;
		IMemPool* _gobal_memory_pool = nullptr;

	};
}

#endif // !_IENGINETOOLS_

