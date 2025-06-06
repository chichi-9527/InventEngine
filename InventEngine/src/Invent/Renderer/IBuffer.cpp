#include "IEpch.h"
#include "IBuffer.h"

namespace INVENT
{
	IBufferLayout::IBufferLayout(std::initializer_list<IBufferElement> elements)
		: _elements(elements)
	{
		CalculateOffsetsAndStride();
	}

	void IBufferLayout::CalculateOffsetsAndStride()
	{
		size_t offset = 0;
		_stride = 0;
		for (auto& element : _elements)
		{
			element.Offset = offset;
			offset += element.Size;
			_stride += element.Size;
		}
	}

	/// <summary>
	/// //////////////////////////////////////////////////////////////////////
	/// </summary>

	IVertexBuffer::IVertexBuffer(unsigned int size)
	{
#ifdef USE_OPENGL
		glCreateBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
#endif // USE_OPENGL
	}

	IVertexBuffer::IVertexBuffer(float* vertices, unsigned int size)
	{
#ifdef USE_OPENGL
		glCreateBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
#endif // USE_OPENGL
	}

	IVertexBuffer::~IVertexBuffer()
	{
#ifdef USE_OPENGL
		glDeleteBuffers(1, &_vbo);
#endif // USE_OPENGL
	}

	void IVertexBuffer::Bind() const
	{
#ifdef USE_OPENGL
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
#endif // USE_OPENGL
	}

	void IVertexBuffer::UnBind() const
	{
#ifdef USE_OPENGL
		glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif // USE_OPENGL
	}

	void IVertexBuffer::SetData(const void* data, unsigned int size)
	{
#ifdef USE_OPENGL
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
#endif // USE_OPENGL
	}

	/// <summary>
	/// //////////////////////////////////////////////////////////////////////
	/// </summary>


	IIndexBuffer::IIndexBuffer(unsigned int* indices, unsigned int count)
		: _count(count)
	{
#ifdef USE_OPENGL
		glCreateBuffers(1, &_ebo);

		glBindBuffer(GL_ARRAY_BUFFER, _ebo);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
#endif // USE_OPENGL
	}

	IIndexBuffer::~IIndexBuffer()
	{
#ifdef USE_OPENGL
		glDeleteBuffers(1, &_ebo);
#endif // USE_OPENGL
	}

	void IIndexBuffer::Bind() const
	{
#ifdef USE_OPENGL
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
#endif // USE_OPENGL
	}

	void IIndexBuffer::UnBind() const
	{
#ifdef USE_OPENGL
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif // USE_OPENGL
	}

}