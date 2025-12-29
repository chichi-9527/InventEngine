#include "IEpch.h"
#include "IShaderStorageBuffer.h"

namespace INVENT
{
	IShaderStorageBuffer::IShaderStorageBuffer(unsigned int size)
	{
#ifdef USE_OPENGL
		glCreateBuffers(1, &_ssbo);
		glNamedBufferData(_ssbo, size, nullptr, GL_DYNAMIC_DRAW);
#endif // USE_OPENGL
	}

	IShaderStorageBuffer::~IShaderStorageBuffer()
	{
#ifdef USE_OPENGL
		glDeleteBuffers(1, &_ssbo);
#endif // USE_OPENGL
	}

	void IShaderStorageBuffer::SetData(const void* data, unsigned int size, unsigned int offset) const
	{
#ifdef USE_OPENGL
		glNamedBufferSubData(_ssbo, offset, size, data);
#endif // USE_OPENGL
	}

	void IShaderStorageBuffer::Bind(unsigned int binding) const
	{
#ifdef USE_OPENGL
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, _ssbo);
#endif // USE_OPENGL
	}

	std::shared_ptr<IShaderStorageBuffer> IShaderStorageBuffer::CreatePtr(unsigned int size)
	{
		return std::make_shared<IShaderStorageBuffer>(size);
	}


}
