#ifndef _ISHADERSTORAGEBUFFER_
#define _ISHADERSTORAGEBUFFER_

namespace INVENT
{
	class IShaderStorageBuffer
	{
	public:
		IShaderStorageBuffer(unsigned int size);
		~IShaderStorageBuffer();

		void SetData(const void* data, unsigned int size, unsigned int offset = 0) const;

		void Bind(unsigned int binding) const;

		static std::shared_ptr<IShaderStorageBuffer> CreatePtr(unsigned int size);

	private:
		unsigned int _ssbo = 0;

	};

}

#endif // !_ISHADERSTORAGEBUFFER_

