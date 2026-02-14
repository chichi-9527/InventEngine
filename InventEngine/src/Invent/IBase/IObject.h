#ifndef _IOBJECT_
#define _IOBJECT_

#include "IObjectBase.h"

namespace INVENT
{

	class IObject : public IObjectBase
	{
	public:
		IObject();
		virtual ~IObject();

		virtual void SetParent(IObject* parent);
		virtual IObject* GetParent() const { return _parent; }

		virtual void AddChild(IObject* child);
		virtual void AddChildren(const std::vector<IObject*>& children);
		virtual void EraseChild(IObject* child);
		virtual const std::vector<IObject*>& GetChildren() const { return _children; }		

		virtual void SetWorldPosition(const glm::vec3& position) override;

		virtual void SetRelativePosition(const glm::vec3& position);
		virtual const glm::vec3& GetRelativePosition();

		virtual void UpdateWorldPositionAboutParent();
		virtual void UpdateRelativePositionAboutParent();

	private:
		std::vector<IObject*> _children;

		IObject* _parent;

	};
}

#endif // !_IOBJECT_
