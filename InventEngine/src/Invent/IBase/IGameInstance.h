#ifndef _IGAMEINSTANCE_
#define _IGAMEINSTANCE_

#include <memory>

#define INVENT_GAMEINSTANCE(class_name) friend class IGameInstance<class_name>;	\
class_name() = default;	\
class_name(const class_name&) = delete;	\
class_name& operator=(const class_name&) = delete;

namespace INVENT
{
	class IBaseGameInstance
	{
	public:
		virtual ~IBaseGameInstance() = default;
		// 游戏开始时调用
		virtual void Begin() = 0;
		virtual void Update(float delta) = 0;
		// 游戏结束时才调用
		virtual void End() = 0;
	protected:
		IBaseGameInstance() = default;
	};

	template<typename T>
	class IGameInstance : public IBaseGameInstance
	{
	public:
		virtual ~IGameInstance() = default;

		IGameInstance(const IGameInstance&) = delete;
		IGameInstance& operator=(const IGameInstance&) = delete;

		static std::shared_ptr<T>& GetGameInstancePtr()
		{
			static std::shared_ptr<T> instance(new T());
			return instance;
		}

	protected:

		IGameInstance() = default;

	};
}

#endif // !_IGAMEINSTANCE_