#ifndef _IITEXTURE_
#define _IITEXTURE_

#include <string>
#include <vector>
#include <unordered_map>

#include <thread>
#include <mutex>

namespace INVENT
{
	class ITexture2D 
	{
		friend class ITexture2DManagement;
	public:
		~ITexture2D();

		struct _UInt2
		{
			unsigned int width;
			unsigned int height;
			bool is_valid;

			_UInt2(unsigned int w = 0, unsigned int h = 0)
				: width(0)
				, height(0)
				, is_valid(true)
			{}

			bool operator==(unsigned int value) const
			{
				return width == value && height == value;
			}

			bool operator==(const _UInt2& other) const
			{
				return width == other.width && height == other.height;
			}

			bool IsZore() const { return (*this) == 0; }
		};

		void Bind() const;
		void BindUnit(unsigned int slot = 0) const;
		const std::string& Name() const { return _name; }

		const _UInt2& GetBreakNum() const { return _texture_breakup; }
		const unsigned int& GetBreakWNum() const { return _texture_breakup.width; }
		const unsigned int& GetBreakHNum() const { return _texture_breakup.height; }

	private:
		ITexture2D();
		ITexture2D(const std::string& name, const std::string& path, const _UInt2& breakup = _UInt2());

	private:
		std::string _name;

		_UInt2 _texture_breakup;

		unsigned int _width;
		unsigned int _height;

		unsigned int _texture_id = 0;

	};

	class ITexture2DManagement 
	{
	public:
		~ITexture2DManagement();

		typedef size_t TextureID;

		static ITexture2DManagement& Instance();

		ITexture2D* CreateTexture(const std::string& path, unsigned int tex_break_width_num = 0, unsigned int tex_break_height_num = 0);
		ITexture2D* CreateTexture(const std::string& name, const std::string& path, unsigned int tex_break_width_num = 0, unsigned int tex_break_height_num = 0);

		TextureID CreateTextureDynamic(const std::string& path, unsigned int tex_break_width_num = 0, unsigned int tex_break_height_num = 0);
		TextureID CreateTextureDynamic(const std::string& name, const std::string& path, unsigned int tex_break_width_num = 0, unsigned int tex_break_height_num = 0);

		ITexture2D* CreateWhiteTexture();

		ITexture2D* GetTexture(const std::string& name);
		ITexture2D* GetTexture(TextureID id)
		{
			return (*this)[id];
		}

		ITexture2D* operator[](TextureID id)
		{
			if (_vector_textrues.size() > id)
				return _vector_textrues[id];
			return nullptr;
		}

		static ITexture2D* GetWhiteTexture();

	private:
		ITexture2DManagement();

	private:
		std::unordered_map<std::string, ITexture2D*> _textrues;
		std::vector<ITexture2D*> _vector_textrues;

		std::mutex _mutex;

	};
}

#endif // !_IITEXTURE_

