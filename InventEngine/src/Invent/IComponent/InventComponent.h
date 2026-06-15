#ifndef _INVENTCOMPONENT_
#define _INVENTCOMPONENT_

#include <glm/glm.hpp>

#include <vector>
#include <atomic>
#include <utility>
#include <concepts>

namespace INVENT
{
	struct IActionComponent 
	{
		IActionComponent() = default;
		IActionComponent(const IActionComponent&) = default;

		virtual void Update(float delta) = 0;
	};

	struct WorldPositionComponent
	{
		glm::vec3 WorldPosition;

		WorldPositionComponent() = default;
		WorldPositionComponent(const WorldPositionComponent&) = default;
		WorldPositionComponent(const glm::vec3& position)
			: WorldPosition(position){}
	};

	struct RelativePositionComponent
	{
		glm::vec3 RelativePosition;

		RelativePositionComponent() = default;
		RelativePositionComponent(const RelativePositionComponent&) = default;
		RelativePositionComponent(const glm::vec3& position)
			: RelativePosition(position){}
	};

	struct RotationComponent
	{
		glm::vec3 Rotation;

		RotationComponent() = default;
		RotationComponent(const RotationComponent&) = default;
		RotationComponent(const glm::vec3& rotation)
			: Rotation(rotation)
		{}
	};

	struct WorldRotationComponent
	{
		glm::vec3 Rotation;

		WorldRotationComponent() = default;
		WorldRotationComponent(const WorldRotationComponent&) = default;
		WorldRotationComponent(const glm::vec3& rotation)
			: Rotation(rotation)
		{}
	};

	struct ScaleComponent
	{
		glm::vec3 Scale;

		ScaleComponent() = default;
		ScaleComponent(const ScaleComponent&) = default;
		ScaleComponent(const glm::vec3& scale)
			: Scale(scale)
		{}
	};


// 注册成员变量，自动生成线程安全的读写函数
// 在移动构造中使用 MoveBuildCode
// 若有需要自行处理值构造与析构函数
#define RegisterMemberVariable(Type, Name)\
		private:\
			Type _##Name[2];\
			alignas(64)std::atomic_int _read_index_##Name{0};\
		public:\
			[[nodiscard]] Type Read##Name() const noexcept\
		{\
		int r_idx = _read_index_##Name.load(std::memory_order_acquire); \
		return _##Name[r_idx]; \
		}\
		template<typename Func>\
		requires std::invocable<Func, Type&>\
		void Write##Name(Func&& modify_func) noexcept\
		{\
			int r_idx = _read_index_##Name.load(std::memory_order_relaxed); \
			int w_idx = 1 - r_idx; \
			_##Name[w_idx] = _##Name[r_idx]; \
			std::forward<Func>(modify_func)(_##Name[w_idx]); \
			_read_index_##Name.store(w_idx, std::memory_order_release); \
		}
				


#define MoveBuildCode(VarName)\
		{\
			_##VarName[0] = other._##VarName[0];\
			_##VarName[1] = other._##VarName[1];\
			int latest_idx = other._read_index_##VarName.load(std::memory_order_acquire);\
			_read_index_##VarName.store(latest_idx, std::memory_order_relaxed);\
		}


	class TestSafeComponent
	{
		RegisterMemberVariable(int, A)
	public:
		~TestSafeComponent(){}
		TestSafeComponent() = default;
		TestSafeComponent(const int& a)
		{
			_A[0] = a;
			_A[1] = a;
		}
		TestSafeComponent(TestSafeComponent&& other) noexcept
		{
			MoveBuildCode(A);
		}
		TestSafeComponent(const TestSafeComponent&) = delete;
		TestSafeComponent& operator=(const TestSafeComponent&) = delete;

		TestSafeComponent& operator=(TestSafeComponent&& other) noexcept
		{
			MoveBuildCode(A);

			return *this;
		}
		 
	private:
		
	};

}




#endif // !_INVENTCOMPONENT_
