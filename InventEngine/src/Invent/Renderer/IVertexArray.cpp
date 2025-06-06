#include "IEpch.h"
#include "IVertexArray.h"

namespace INVENT
{
	IVertexArray::IVertexArray()
	{
#ifdef USE_OPENGL
		glCreateVertexArrays(1, &_vao);
#endif // USE_OPENGL
	}

	IVertexArray::~IVertexArray()
	{
#ifdef USE_OPENGL
		glDeleteVertexArrays(1, &_vao);
#endif // USE_OPENGL
	}

	void IVertexArray::Bind() const
	{
#ifdef USE_OPENGL
		glBindVertexArray(_vao);
#endif // USE_OPENGL
	}

	void IVertexArray::UnBind() const
	{
#ifdef USE_OPENGL
		glBindVertexArray(0);
#endif // USE_OPENGL
	}

	void IVertexArray::AddVertexBuffer(const std::shared_ptr<IVertexBuffer>& vertex_buffer)
	{
		INVENT_ASSERT(vertex_buffer->GetLayout().GetElement().size(), "Vertex Buffer has no layout!");

		_vertex_buffer_index = 0;

#ifdef USE_OPENGL
		glBindVertexArray(_vao);
		vertex_buffer->Bind();

		const auto& layout = vertex_buffer->GetLayout();
		for (const auto& element : layout.GetElement())
		{
			switch (element.Type)
			{
			case IShaderDataType::Float:
			case IShaderDataType::Float2:
			case IShaderDataType::Float3:
			case IShaderDataType::Float4: 
				glEnableVertexAttribArray(_vertex_buffer_index);
				glVertexAttribPointer(_vertex_buffer_index, element.GetComponentCount(), GL_FLOAT, element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)element.Offset);
				++_vertex_buffer_index;
				break;
			case IShaderDataType::Int:
			case IShaderDataType::Int2:
			case IShaderDataType::Int3:
			case IShaderDataType::Int4:
				glEnableVertexAttribArray(_vertex_buffer_index);
				glVertexAttribIPointer(_vertex_buffer_index, element.GetComponentCount(), GL_INT, layout.GetStride(), (const void*)element.Offset);
				++_vertex_buffer_index;
				break;
			case IShaderDataType::Bool:
				glEnableVertexAttribArray(_vertex_buffer_index);
				glVertexAttribIPointer(_vertex_buffer_index, element.GetComponentCount(), GL_BOOL, layout.GetStride(), (const void*)element.Offset);
				++_vertex_buffer_index;
				break;
			case IShaderDataType::Mat3:
			case IShaderDataType::Mat4:
				for (size_t i = 0; i < element.GetComponentCount(); ++i)
				{
					glEnableVertexAttribArray(_vertex_buffer_index);
					glVertexAttribPointer(_vertex_buffer_index, element.GetComponentCount(), GL_FLOAT, element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)(element.Offset + sizeof(float) * element.GetComponentCount() * i));
					glVertexAttribDivisor(_vertex_buffer_index, 1);
					++_vertex_buffer_index;
				}
				break;
			default:
				INVENT_ASSERT(false, "Unknown ShaderDataType!");
			}
		}
		_vertex_buffers.push_back(vertex_buffer);

#endif // USE_OPENGL
	}

	void IVertexArray::SetIndexBuffer(const std::shared_ptr<IIndexBuffer>& index_buffer)
	{
#ifdef USE_OPENGL
		glBindVertexArray(_vao);
		index_buffer->Bind();
#endif // USE_OPENGL
	
		_index_buffer = index_buffer;

	}

}