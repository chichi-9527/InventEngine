#ifndef _IOBJECT2D_
#define _IOBJECT2D_

#include "IBase/IObjectBase.h"

namespace INVENT
{
	class IObject2D : public IObjectBase
	{
	public:
		IObject2D();
		virtual ~IObject2D();

		//void SetParent(IObject2D* parent);
		//IObject2D* GetParent() const { return _parent; }

		//void AddChild(IObject2D* child);
		//void AddChildren(const std::vector<IObject2D*>& children);
		//void EraseChild(IObject2D* child);
		//const std::vector<IObject2D*>& GetChildren() const { return _children; }

		//virtual void SetWorldPosition(const glm::vec3& position) override;

		//void SetRelativePosition(const glm::vec2& position);
		//const glm::vec2& GetRelativePosition();

	private:
		IObject2D* _parent;

		std::vector<IObject2D*> _children;
	};
}

#endif // !_IOBJECT2D_
