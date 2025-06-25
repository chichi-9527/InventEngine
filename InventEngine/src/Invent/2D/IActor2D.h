#ifndef _IACTOR2D_
#define _IACTOR2D_

#include "IObject2D.h"

#include "IBase/IBaseActor.h"

#include "IPhysicsCollision/ICollider.h"

namespace INVENT
{
	class IActor2D : public IObject2D, public IBaseActor
	{
	public:
		IActor2D();
		virtual ~IActor2D();

	private:

	};

}

#endif // !_IACTOR2D_

