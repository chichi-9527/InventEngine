#ifndef _ISQUARE2DPAWN_
#define _ISQUARE2DPAWN_

#include "ISquare2dActor.h"
#include "Invent/IBase/IBasePawnControl.h"

namespace INVENT
{
	/// <summary>
	/// IPwan2D 作为 IPlayerController2D 移动事件的基类
	/// 由于分出了 2d actor 使得 Controller 不通用（不得不说这是设计的失误）
	/// 正常来说在 3d 世界中绘制 2d 模型只需像 IActor3D 那样添加 2D Mesh 即可
	/// </summary>
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
