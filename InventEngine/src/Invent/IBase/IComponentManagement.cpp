#include "IEpch.h"
#include "IComponentManagement.h"

namespace INVENT
{

	IComponentManagement::IComponentManagement()
	{}

	IComponentManagement::~IComponentManagement()
	{
		for (auto& iter : _b_arr_set)
		{
			iter->Remove(this);
		}
		_b_arr_set.clear();
	}

	IComponentManagement::Entity IComponentManagement::Create()
	{
		for (size_t i = 0; i < _handles.size(); ++i)
		{
			if (_handles[i] == 0)
			{
				_handles[i] = 1;
				return i;
			}	
		}
		_handles.push_back(1);
		return _handles.size() - 1;
	}

	void IComponentManagement::Remove(Entity t)
	{
		_handles[t] = 0;
	}
}