#ifndef _IMESH_
#define _IMESH_

#include "IBase/IObjectBase.h"


namespace INVENT
{
	struct MeshComponent;
	class IMesh :
		public IObjectBase
	{
	public:
		IMesh();
		virtual ~IMesh();

		// no invalid
		virtual void SetWorldPosition(const glm::vec3&) override{}

		MeshComponent* GetMesh();


	private:

	};


}



#endif // !_IMESH_