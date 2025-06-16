#ifndef _IBUFFER_
#define _IBUFFER_

#include <vector>
#include <string>

namespace INVENT
{
	enum class IShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static uint32_t ShaderDataTypeSize(IShaderDataType type)
	{
		switch (type)
		{
		case IShaderDataType::Float:    return 4;
		case IShaderDataType::Float2:   return 4 * 2;
		case IShaderDataType::Float3:   return 4 * 3;
		case IShaderDataType::Float4:   return 4 * 4;
		case IShaderDataType::Mat3:     return 4 * 3 * 3;
		case IShaderDataType::Mat4:     return 4 * 4 * 4;
		case IShaderDataType::Int:      return 4;
		case IShaderDataType::Int2:     return 4 * 2;
		case IShaderDataType::Int3:     return 4 * 3;
		case IShaderDataType::Int4:     return 4 * 4;
		case IShaderDataType::Bool:     return 1;
		}

		INVENT_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	struct IBufferElement
	{
		std::string Name;
		IShaderDataType Type;
		unsigned int Size;
		size_t Offset;
		bool Normalized;

		IBufferElement() = default;
		IBufferElement(IShaderDataType type, const std::string& name, bool normalized = false)
			: Type(type)
			, Name(name)
			, Size(ShaderDataTypeSize(type))
			, Offset(0)
			, Normalized(normalized)
		{}

		unsigned int GetComponentCount() const
		{
			switch (Type)
			{
			case INVENT::IShaderDataType::Float:	return 1;
			case INVENT::IShaderDataType::Float2:	return 2;
			case INVENT::IShaderDataType::Float3:	return 3;
			case INVENT::IShaderDataType::Float4:	return 4;
			case INVENT::IShaderDataType::Mat3:		return 3;
			case INVENT::IShaderDataType::Mat4:		return 4;
			case INVENT::IShaderDataType::Int:		return 1;
			case INVENT::IShaderDataType::Int2:		return 2;
			case INVENT::IShaderDataType::Int3:		return 3;
			case INVENT::IShaderDataType::Int4:		return 4;
			case INVENT::IShaderDataType::Bool:		return 1;
			}
			INVENT_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}
	};

	class IBufferLayout 
	{
	public:
		IBufferLayout(){}
		IBufferLayout(std::initializer_list<IBufferElement> elements);

		unsigned int GetStride() const { return _stride; }
		const std::vector<IBufferElement>& GetElement() const { return _elements; }

	private:
		void CalculateOffsetsAndStride();
	private:
		std::vector<IBufferElement> _elements;
		unsigned int _stride = 0;
	};

	// VBO
	class IVertexBuffer 
	{
	public:
		IVertexBuffer(unsigned int size);
		IVertexBuffer(float* vertices, unsigned int size);
		~IVertexBuffer();

		void Bind() const;
		void UnBind() const;

		void SetData(const void* data, unsigned int size);

		const IBufferLayout& GetLayout() const { return _layout; }
		void SetLayout(const IBufferLayout& layout) { _layout = layout; }

		static std::shared_ptr<IVertexBuffer> CreatePtr(unsigned int size);
		static std::shared_ptr<IVertexBuffer> CreatePtr(float* vertices, unsigned int size);

	private:
		unsigned int _vbo = 0;
		IBufferLayout _layout;
	};

	// EBO/IBO
	class IIndexBuffer 
	{
	public:
		IIndexBuffer(unsigned int* indices, unsigned int count);
		~IIndexBuffer();

		void Bind() const;
		void UnBind() const;

		unsigned int GetCount() const { return _count; }

		static std::shared_ptr<IIndexBuffer> CreatePtr(unsigned int* indices, unsigned int count);

	private:
		unsigned int _ebo = 0;
		unsigned int _count;
	};
}

#endif // !_IBUFFER_
