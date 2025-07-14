#include "IDelegate.h"

namespace INVENT
{
	static std::shared_ptr<IThreadPool> delegate_event_thread_pool_ptr(new IThreadPool());

	std::shared_ptr<IThreadPool> ITools::GetDelegateEventThreadPool()
	{
		delegate_event_thread_pool_ptr->Start();
		return delegate_event_thread_pool_ptr;
	}
}


