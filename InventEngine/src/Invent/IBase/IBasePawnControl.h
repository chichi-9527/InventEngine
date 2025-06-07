#ifndef _IBASEPAWNCONTROL_
#define _IBASEPAWNCONTROL_

namespace INVENT
{
	class IBasePawnControl2D
	{
	public:
		IBasePawnControl2D() = default;
		virtual ~IBasePawnControl2D() = default;

		void SetMoveSpeed(float speed) { MoveSpeed = speed; }
		float GetMoveSpeed() { return MoveSpeed; }

		virtual void MoveUp() = 0;
		virtual void MoveDown() = 0;
		virtual void MoveLeft() = 0;
		virtual void MoveRight() = 0;

	private:
		float MoveSpeed = 0.0f;
	};

	class IBasePawnControl : public IBasePawnControl2D
	{
	public:
		IBasePawnControl() = default;
		virtual ~IBasePawnControl() = default;

		virtual void MoveForward() = 0;
		virtual void MoveBackward() = 0;

	};
}

#endif // !_IBASEPAWNCONTROL_

