#include "IEpch.h"
#include "ILevel.h"

namespace INVENT
{
	ILevel::ILevel(const glm::vec3& position)
		: IBaseLevel(position)
	{}
	ILevel::~ILevel()
	{}
	void ILevel::Begin()
	{}
	void ILevel::Update(float delta)
	{}
	void ILevel::End()
	{}
}
