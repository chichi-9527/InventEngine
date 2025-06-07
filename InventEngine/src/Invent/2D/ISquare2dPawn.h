#ifndef _ISQUARE2DPAWN_
#define _ISQUARE2DPAWN_

#include "ISquare2dActor.h"
#include "IBase/IBasePawnControl.h"

namespace INVENT
{

	class ISquare2dPawn : public ISquare2dActor, IBasePawnControl2D
	{
	public:
		ISquare2dPawn();
		virtual ~ISquare2dPawn();

		virtual void MoveUp() override;
	};

}

#endif // !_ISQUARE2DPAWN_
