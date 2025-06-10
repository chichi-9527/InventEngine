#include "IEpch.h"
#include "ICamera.h"

namespace INVENT
{
	ICamera::ICamera()
		: IObjectBase()
		, IBaseActor()
		, IBasePawnControl()
	{
		this->SetForwardVector({ 0.0f,0.0f,-1.0f });
		this->SetUpVector({ 0.0f,1.0f,0.0f });


	}

	ICamera::~ICamera()
	{}
}