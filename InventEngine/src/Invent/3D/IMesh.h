#ifndef _IMESH_
#define _IMESH_

#include "IBase/IObjectBase.h"

#include <string>

namespace INVENT
{
	struct MeshComponent;
	class IMesh :
		public IObjectBase
	{
		friend class ILoadModel;
	public:
		IMesh();
		virtual ~IMesh();

		// no invalid
		virtual void SetWorldPosition(const glm::vec3&) override{}

		MeshComponent* GetMesh();

		const std::string& Name() const { return _name; }
	private:
		std::string _name;
	};


}



#endif // !_IMESH_