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

		virtual void SetParent(IObject2D* parent);
		virtual IObject2D* GetParent() const { return _parent; }

		virtual void AddChild(IObject2D* child);
		virtual void AddChildren(const std::vector<IObject2D*>& children);
		virtual void EraseChild(IObject2D* child);
		virtual const std::vector<IObject2D*>& GetChildren() const { return _children; }

		virtual void SetWorldPosition(const glm::vec3& position) override;
		virtual const glm::vec3& GetWorldPosition() override;

		virtual void SetRelativePosition(const glm::vec2& position);
		virtual const glm::vec2& GetRelativePosition();

	private:
		std::vector<IObject2D*> _children;

		IObject2D* _parent;
	};
}

#endif // !_IOBJECT2D_
