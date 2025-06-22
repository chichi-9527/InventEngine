#include "IEpch.h"
#include "IUniformBuffer.h"

namespace INVENT
{
	IUniformBuffer::IUniformBuffer(unsigned int size, unsigned int binding)
	{
#ifdef USE_OPENGL
		glCreateBuffers(1, &_ubo);
		glNamedBufferData(_ubo, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, _ubo);
#endif // USE_OPENGL
	}

	IUniformBuffer::~IUniformBuffer()
	{
#ifdef USE_OPENGL
		glDeleteBuffers(1, &_ubo);
#endif // USE_OPENGL
	}

	void IUniformBuffer::SetData(const void* data, unsigned int size, unsigned int offset) const
	{
#ifdef USE_OPENGL
		glNamedBufferSubData(_ubo, offset, size, data);
#endif // USE_OPENGL
	}

	std::shared_ptr<IUniformBuffer> IUniformBuffer::CreatePtr(unsigned int size, unsigned int binding)
	{
		return std::make_shared<IUniformBuffer>(size, binding);
	}
}