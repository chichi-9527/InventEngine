#ifndef _ICAMERA_
#define _ICAMERA_

#include "IObjectBase.h"
#include "IBaseActor.h"
#include "IBasePawnControl.h"

#include <glm/glm.hpp>

namespace INVENT
{
	class ICamera : public IObjectBase, IBaseActor, IBasePawnControl
	{
	public:
		ICamera();
		virtual ~ICamera();

	private:

	};
}

#endif // !_ICAMERA_
