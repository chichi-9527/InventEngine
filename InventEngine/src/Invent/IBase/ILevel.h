#ifndef _ILEVEL_
#define _ILEVEL

#include "IBaseLevel.h"

namespace INVENT
{
	class ILevel : public IBaseLevel
	{
	public:
		ILevel(const glm::vec3& position = {});
		virtual ~ILevel();

		virtual void Begin() override;
		virtual void Update(float delta) override;
		virtual void End() override;

	private:

	};
}

#endif // !_ILEVEL_
