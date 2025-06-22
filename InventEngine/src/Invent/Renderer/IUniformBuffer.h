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
		~IUniformBuffer();

		void SetData(const void* data, unsigned int size, unsigned int offset = 0) const;

		static std::shared_ptr<IUniformBuffer> CreatePtr(unsigned int size, unsigned int binding);

	private:
		unsigned int _ubo = 0;
	};
}

#endif // !_IUNIFORMBUFFER_
