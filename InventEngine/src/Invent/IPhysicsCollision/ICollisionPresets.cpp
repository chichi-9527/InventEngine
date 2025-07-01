#include "ICollisionPresets.h"

#include "ILog.h"

namespace INVENT
{
	template<typename T>
	struct SymmetricMatrix 
	{
		// if n == 0; n = 1
		SymmetricMatrix(size_t n)
			: N(n < 1 ? 1 : n)
		{
			_table.resize(n * (n - 1) / 2 + n);
		}

		T& operator()(size_t i, size_t j)
		{
			if (i >= N || j >= N)
			{
				INVENT_LOG_ERROR("SymmetricMatrix i or j out of range ");
				return _table[0];
			}
			if (i < j)
				std::swap(i, j);
			return _table[i * (i + 1) / 2 + j];
		}

		T& operator()(size_t i)
		{
			if (i >= N)
			{
				INVENT_LOG_ERROR("SymmetricMatrix i out of range ");
				return _table[0];
			}
			return _table[i * (i + 1) / 2 + i];
		}

		std::vector<T> _table;
		size_t N;
	};

	static SymmetricMatrix<ICollisionPresets::CollisionType> collision_type_matrix(static_cast<size_t>(ICollisionPresets::CollisionPresets::PRESET_END));

	void ICollisionPresets::Init()
	{
		collision_type_matrix._table = { CollisionType::COLLISION_BLOCK };
		INVENT_LOG_DEBUG("if you update enum collision presets you need update _table_collision_type in ICollisionPresets::Init() !");
	}

	const ICollisionPresets::CollisionType& ICollisionPresets::GetCollisionTypeWithTwoCollisionPreset(const CollisionPresets& preset1, const CollisionPresets& preset2)
	{
		return collision_type_matrix(static_cast<size_t>(preset1), static_cast<size_t>(preset2));
	}

	void ICollisionPresets::SetCollisionTypeWithTwoCollisionPreset(const CollisionPresets& preset1, const CollisionPresets& preset2, const CollisionType& type)
	{
		collision_type_matrix(static_cast<size_t>(preset1), static_cast<size_t>(preset2)) = type;
	}


}