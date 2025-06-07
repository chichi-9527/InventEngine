#ifndef _IBASEACTOR_
#define _IBASEACTOR_

namespace INVENT
{
	class IBaseActor 
	{
	public:
		IBaseActor() = default;
		virtual ~IBaseActor() = default;

		virtual void Update(float delta) = 0;
	};
}

#endif // !_IBASEACTOR_

