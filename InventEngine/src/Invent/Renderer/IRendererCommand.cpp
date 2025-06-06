#include "IEpch.h"
#include "IRendererCommand.h"

namespace INVENT
{
	void IRendererCommend::Init()
	{
#ifdef USE_OPENGL
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
#endif // USE_OPENGL
	}

	void IRendererCommend::DrawIndexed(const std::shared_ptr<IVertexArray>& vertex_array, unsigned int index_count)
	{
#ifdef USE_OPENGL
		vertex_array->Bind();
		glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
#endif // USE_OPENGL
	}

	void IRendererCommend::DrawLines(const std::shared_ptr<IVertexArray>& vertex_array, unsigned int vertex_count)
	{
#ifdef USE_OPENGL
		vertex_array->Bind();
		glDrawArrays(GL_LINES, 0, vertex_count);
#endif // USE_OPENGL
	}


	void IRendererCommend::SetLineWidth(float width)
	{
#ifdef USE_OPENGL
		glLineWidth(width);
#endif // USE_OPENGL
	}
}