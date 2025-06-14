#ifndef _ISQUARE2DPAWN_
#define _ISQUARE2DPAWN_

#include "ISquare2dActor.h"
#include "IBase/IBasePawnControl.h"

namespace INVENT
{

	class ISquare2dPawn : public ISquare2dActor,public IBasePawnControl2D
	{
	public:
		ISquare2dPawn();
		virtual ~ISquare2dPawn();

		virtual void MoveUp(float delta) override;
		virtual void MoveDown(float delta) override;
		virtual void MoveLeft(float delta) override;
		virtual void MoveRight(float delta) override;
	};

}

#endif // !_ISQUARE2DPAWN_
