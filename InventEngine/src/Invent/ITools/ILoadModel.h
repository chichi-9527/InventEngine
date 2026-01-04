#ifndef _ILOADMODEL_
#define _ILOADMODEL_

#include <vector>
#include <string>

namespace INVENT
{
	class IMesh;
	class ILoadModel
	{
	public:
		static void Load(std::vector<IMesh>& meshes, const std::string& path);

	};


}

#endif // !_ILOADMODEL_

