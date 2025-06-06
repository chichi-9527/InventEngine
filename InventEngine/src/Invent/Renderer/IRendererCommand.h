#ifndef _IRENDERERCOMMAND_
#define _IRENDERERCOMMAND_

#include "IVertexArray.h"

namespace INVENT
{

	class IRendererCommend 
	{
	public:
		static void Init();

		static void DrawIndexed(const std::shared_ptr<IVertexArray>& vertex_array, unsigned int index_count);
		static void DrawLines(const std::shared_ptr<IVertexArray>& vertex_array, unsigned int vertex_count);

		static void SetLineWidth(float width);
	};

}

#endif // !_IRENDERERCOMMAND_
