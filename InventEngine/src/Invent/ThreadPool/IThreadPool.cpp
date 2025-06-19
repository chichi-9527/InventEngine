#include "IThreadPool.h"

namespace INVENT
{
	IThreadWork::IThreadWork(IThreadPool* pool)
		: _pool(pool)
	{}

	IThreadWork::~IThreadWork()
	{}

	////////////////////////////////////////////////////////////////////////////////////////////
	////////////// thread pool
	////////////////////////////////////////////////////////////////////////////////////////////

	IThreadPool::IThreadPool(unsigned int thread_num, unsigned int priority_num)
		: _thread_num(thread_num)
		, _priority_num(priority_num)
		, _is_shutdown(false)
	{
		for (unsigned int i = 0; i < thread_num; ++i)
		{
			_func_queues.push_back(new IFunctionQueue<std::function<void()>>);
		}
	}

	IThreadPool::~IThreadPool()
	{
		Shutdown();

		for (auto func_queue : _func_queues)
			if (func_queue) delete func_queue;
	}

	void IThreadPool::Start()
	{
		for (unsigned int i = 0; i < _thread_num; ++i)
		{
			_threads.push_back(new std::thread([this]() {IThreadWork work(this); work.Start(); }));
		}
	}

	void IThreadPool::Shutdown() 
	{
		_is_shutdown = true; 
		_lock.notify_all();

		for (auto thread : _threads)
			if (thread->joinable()) thread->join();

		for (auto thread : _threads)
			delete thread;
	}


}