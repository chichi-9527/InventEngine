#ifndef _ICOMPONENTMANAGEMENT_
#define _ICOMPONENTMANAGEMENT_

#include "ILog.h"

#include <vector>
#include <utility>
#include <unordered_set>

#include <memory>

namespace INVENT
{
	struct IBool
	{
		IBool()
			: value(false)
		{}
		IBool(bool v)
			: value(v)
		{}

		bool value;
	};

	class IComponentManagement;

	class IBaseComponentManagementArray 
	{
	public:
		IBaseComponentManagementArray(){}
		virtual ~IBaseComponentManagementArray(){}

		virtual void Remove(IComponentManagement* component) = 0;
	};

	template<typename T>
	class IComponentManagementArray : public IBaseComponentManagementArray
	{
	public:
		virtual ~IComponentManagementArray()
		{
			for (auto vec : _vec)
			{
				delete vec;
			}
		}

		static std::shared_ptr<IComponentManagementArray<T>> Instance()
		{
			static std::shared_ptr<IComponentManagementArray<T>> component_management_ptr(new IComponentManagementArray<T>);
			return  component_management_ptr;
		}

		std::vector<std::pair<T, IBool>>* GetComponentArray(IComponentManagement* component)
		{
			for (size_t i = 0; i < _component_managements.size(); ++i)
			{
				if (_component_managements[i] == component)
				{
					if (_vec.size() < i + 1)
					{
						_vec.resize(i + 1, new std::vector<std::pair<T, IBool>>);
					}
					return _vec[i];
				}
			}
			_component_managements.push_back(component);
			if (_vec.size() < _component_managements.size())
			{
				_vec.resize(_component_managements.size(), new std::vector<std::pair<T, IBool>>);
			}
			return _vec[_component_managements.size() - 1];;
		}

		std::vector<std::pair<T, IBool>>* HasComponentArray(IComponentManagement* component)
		{
			for (size_t i = 0; i < _component_managements.size(); ++i)
			{
				if (_component_managements[i] == component)
				{
					if (_vec.size() < i + 1)
					{
						_vec.resize(i + 1, new std::vector<std::pair<T, IBool>>);
					}
					return _vec[i];
				}
			}
			return nullptr;
		}

		virtual void Remove(IComponentManagement* component) override
		{
			for (size_t i = 0; i < _component_managements.size(); ++i)
			{
				if (_component_managements[i] == component)
				{
					INVENT_LOG_INFO(std::string("REMOVE COMPONENT MANAGEMENT ") + std::to_string((long)component));
					delete _vec[i];
					_vec.erase(_vec.begin() + i);
					_component_managements.erase(_component_managements.begin() + i);
					break;
				}
			}
		}

	private:
		IComponentManagementArray() = default;

	private:
		std::vector<std::vector<std::pair<T, IBool>>*> _vec;
		std::vector<IComponentManagement*> _component_managements;
	};

	class IComponentManagement 
	{
	public:
		typedef size_t Entity;

		IComponentManagement();
		~IComponentManagement();

		Entity Create();
		void Remove(Entity t);

		template<typename T, typename... Args>
		T& Emplace(Entity t, Args&&... args)
		{
			_b_arr_set.insert(std::dynamic_pointer_cast<IBaseComponentManagementArray>(IComponentManagementArray<T>::Instance()));

			std::vector<std::pair<T, IBool>>*  component_array = IComponentManagementArray<T>::Instance()->GetComponentArray(this);
			if (component_array->size() < t + 1)
			{
				component_array->resize(t + 1);
			}
			(*component_array)[t] = std::make_pair(T(std::forward<Args>(args)...), IBool(true));
			return (*component_array)[t].first;
		}

		template<typename T>
		T* Get(Entity t)
		{
			std::vector<std::pair<T, IBool>>* component_array = IComponentManagementArray<T>::Instance()->HasComponentArray(this);
			if (component_array)
			{
				if (component_array->size() < t + 1)
				{
					return nullptr;
				}
				if ((*component_array)[t].second.value)
					return &(*component_array)[t].first;
			}
			return nullptr;
		}

		template<typename T>
		T& GetNotSafe(Entity t)
		{
			std::vector<std::pair<T, IBool>>* component_array = IComponentManagementArray<T>::Instance()->HasComponentArray(this);

			return (*component_array)[t].first;

		}

		template<typename T>
		bool Has(Entity t)
		{
			std::vector<std::pair<T, IBool>>* component_array = IComponentManagementArray<T>::Instance()->HasComponentArray(this);
			if (component_array)
			{
				if (component_array->size() < t + 1)
				{
					return false;
				}
				return (*component_array)[t].second.value;
			}
			return false;
		}

		template<typename T>
		void Remove(Entity t)
		{
			std::vector<std::pair<T, IBool>>* component_array = IComponentManagementArray<T>::Instance()->HasComponentArray(this);
			if (component_array)
			{
				if (component_array->size() < t + 1)
				{
					return;
				}
				(*component_array)[t].second.value = false;
			}
		}

	private:
		std::vector<int> _handles;
		std::unordered_set< std::shared_ptr<IBaseComponentManagementArray>> _b_arr_set;
	};



}


#endif // !_ICOMPONENTMANAGEMENT_
