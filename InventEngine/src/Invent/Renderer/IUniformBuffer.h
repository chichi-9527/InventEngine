#ifndef _IUNIFORMBUFFER_
#define _IUNIFORMBUFFER_

#include <memory>

namespace INVENT
{
	// UBO
	class IUniformBuffer 
	{
	public:
		IUniformBuffer(unsigned int size, unsigned int binding);
		IUniformBuffer(unsigned int size);
		~IUniformBuffer();

		void SetData(const void* data, unsigned int size, unsigned int offset = 0) const;

		void Bind(unsigned int) const;
		void UnBind(unsigned int) const;

		static std::shared_ptr<IUniformBuffer> CreatePtr(unsigned int size, unsigned int binding);
		static std::shared_ptr<IUniformBuffer> CreatePtr(unsigned int size);

	private:
		unsigned int _ubo = 0;
	};
}

#endif // !_IUNIFORMBUFFER_
