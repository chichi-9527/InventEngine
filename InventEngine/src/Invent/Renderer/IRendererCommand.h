#ifndef _IRENDERERCOMMAND_
#define _IRENDERERCOMMAND_


namespace INVENT
{
	class IVertexArray;
	class IRendererCommend 
	{
	public:
		static void Init();

		// glDrawElements
		static void DrawIndexed(const std::shared_ptr<IVertexArray>& vertex_array, unsigned int index_count);
		static void DrawLines(const std::shared_ptr<IVertexArray>& vertex_array, unsigned int vertex_count);

		// glMultiDrawElementsIndirect
		static void MultiDrawElementsIndirect(const std::shared_ptr<IVertexArray>& vertex_array, unsigned int draw_count);

		static void SetLineWidth(float width);
	};

}

#endif // !_IRENDERERCOMMAND_
