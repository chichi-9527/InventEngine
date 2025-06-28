#include "ICollisionPresets.h"

#include "ILog.h"

namespace INVENT
{
	static std::vector<ICollisionPresets::CollisionType> _table_collision_type;

	void ICollisionPresets::Init()
	{
		_table_collision_type = { CollisionType::COLLISION_IGNORE };
		INVENT_ASSERT(_table_collision_type.size() != static_cast<size_t>(CollisionPresets::PRESET_END), "if you update enum collision presets you must update _table_collision_type!");
	}
	ICollisionPresets::CollisionType ICollisionPresets::GetCollisionType(CollisionPresets preset)
	{
		return _table_collision_type[static_cast<size_t>(preset)];
	}
}