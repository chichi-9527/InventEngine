#ifndef _ISCENE_
#define _ISCENE_

#include "ThreadPool/IThreadPool.h"
#include "IEventLayer.h"

#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <format>

namespace INVENT
{
	class ILevel;
	class IBaseActor;

	class IScene : public IEventLayer
	{
	public:
		struct SLevel
		{
			ILevel* Level = nullptr;
			glm::vec3 Position = {};
		};

		~IScene();

		static std::shared_ptr<IScene> InstancePtr();

		virtual void Update(float delta);

		typedef size_t ILevelID;
		/// <summary>
		/// 创建关卡实例
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <returns>关卡id</returns>
		template<typename T>
		ILevelID CreateLevelInstance()
		{
			return 0;
		}

		/// <summary>
		/// 先创建关卡实例，在将此实例加入显示列表
		/// </summary>
		/// <typeparam name="T">关卡类</typeparam>
		/// <param name="position">关卡实例在场景中的位置</param>
		/// <returns>关卡id</returns>
		template<typename T>
		ILevelID ShowLevelInstance(const glm::vec3& position = { 0.0f })
		{

		}

		
		template<typename T, bool IsRender>
		T* CreateActor()
		{
			if constexpr (!std::is_base_of_v<IBaseActor, T>)
			{
				INVENT_LOG_ERROR(std::format("[IEngine] Window class type error; {}", typeid(T).name()));
				return nullptr;
			}

			auto actor = new T;

			_get_engine_work_thread_pool()->Submit(0, [this, actor]() {
				

				});
		}

	private:
		IScene();

		IThreadPool* _get_engine_work_thread_pool();

	private:
		std::vector<SLevel> _scene_levels;

	};


}
#endif // !_ISCENE_

