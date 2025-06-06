#ifndef _IVERTEXARRAY_
#define _IVERTEXARRAY_

#include "IBuffer.h"

#include <memory>
#include <vector>

namespace INVENT
{
	// VAO
	class IVertexArray
	{
	public:
		IVertexArray();
		~IVertexArray();

		void Bind() const;
		void UnBind() const;

		void AddVertexBuffer(const std::shared_ptr<IVertexBuffer>& vertex_buffer);
		void SetIndexBuffer(const std::shared_ptr<IIndexBuffer>& index_buffer);

		const std::vector<std::shared_ptr<IVertexBuffer>>& GetVertexBuffers() const { return _vertex_buffers; }
		const std::shared_ptr<IIndexBuffer> GetIndexBuffer() const { return _index_buffer; }

	private:
		unsigned int _vao = 0;
		unsigned int _vertex_buffer_index = 0;
		std::vector<std::shared_ptr<IVertexBuffer>> _vertex_buffers;
		std::shared_ptr<IIndexBuffer> _index_buffer;
	};
}

#endif // !_IVERTEXARRAY_

