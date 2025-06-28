#ifndef _ICOLLISIONPRESETS_
#define _ICOLLISIONPRESETS_

#include <vector>

namespace INVENT
{
	class ICollisionPresets
	{
	public:
		static void Init();

		enum class CollisionType
		{
			COLLISION_BLOCK,
			COLLISION_OVERLAP,
			COLLISION_IGNORE
		};

		enum class CollisionPresets : size_t
		{
			PRESET_NORMAL = 0,
			PRESET_END
		};

		static CollisionType GetCollisionType(CollisionPresets preset);

	};
}

#endif // !_ICOLLISIONPRESETS_
