#include "IEpch.h"
#include "ISquare2dPawn.h"

namespace INVENT
{
	ISquare2dPawn::ISquare2dPawn()
	{
		this->SetForwardVector({ 0.0f,0.0f,-1.0f });
		this->SetUpVector({ 0.0f,1.0f,0.0f });
	}

	ISquare2dPawn::~ISquare2dPawn()
	{}

	void ISquare2dPawn::MoveUp()
	{}

	void ISquare2dPawn::MoveDown()
	{}

	void ISquare2dPawn::MoveLeft()
	{}

	void ISquare2dPawn::MoveRight()
	{}
}