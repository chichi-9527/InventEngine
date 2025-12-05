#ifndef _IITEXTURE_
#define _IITEXTURE_

#include <string>
#include <vector>
#include <unordered_map>

#include <thread>
#include <mutex>
#include <atomic>

namespace INVENT
{

	struct CharCharacter {
		unsigned int Width = 0;
		unsigned int Rows = 0;
		unsigned char* Buffer = nullptr;

		unsigned int OffsetLeft = 0;
		unsigned int OffsetTop = 0;		// Offset from baseline to left/top of glyph

		unsigned int AdvanceX = 0;		// Offset to advance to next glyph
		unsigned int AdvanceY = 0;

		bool IsValid() const { return nullptr != Buffer; }
	};

	class ITextureBase 
	{
	public:
		ITextureBase() : _texture_id(0){}
		virtual ~ITextureBase();

		virtual void Bind() const = 0;
		virtual void BindUnit(unsigned int slot = 0) const;
		virtual void InitTextureID() = 0;

	public:
		std::atomic_bool IsValid = false;

	protected:
		unsigned int _texture_id;
	};


	class ITextureCubeMap : public ITextureBase
	{
	public:
		~ITextureCubeMap();

		virtual void Bind() const override;
		virtual void InitTextureID() override;

		const std::string& Name() { return _name; }

	private:
		ITextureCubeMap(const std::string& name, const std::vector<std::string>& face_pathes);


	private:
		std::string _name;

		unsigned int* _width = nullptr;
		unsigned int* _height = nullptr;
		unsigned int* _channels = nullptr;

		unsigned int _face_num = 0;

		unsigned char** _tex_data = nullptr;
	};


	class ITexture2D : public ITextureBase
	{
		friend class ITexture2DManagement;
	public:
		~ITexture2D();

		struct _UInt2
		{
			unsigned int width;
			unsigned int height;
			bool is_valid;

			_UInt2(unsigned int w = 0, unsigned int h = 0, bool valid = true)
				: width(w)
				, height(h)
				, is_valid(valid)
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

		virtual void Bind() const override;
		const std::string& Name() const { return _name; }

		void SetBreakNum(unsigned int w, unsigned int h);
		void SetBreakNum(const _UInt2& break_num);
		const _UInt2& GetBreakNum() const { return _texture_breakup; }
		const unsigned int& GetBreakWNum() const { return _texture_breakup.width; }
		const unsigned int& GetBreakHNum() const { return _texture_breakup.height; }

		virtual void InitTextureID() override;

	private:
		ITexture2D();
		ITexture2D(const std::string& name, const std::string& path, const _UInt2& breakup = _UInt2());
		ITexture2D(const std::string& name, const CharCharacter& character, const _UInt2& breakup = _UInt2());

	private:
		std::string _name;

		_UInt2 _texture_breakup;

		CharCharacter _charcharacter;

		unsigned int _width = 0;
		unsigned int _height = 0;
		unsigned int _channels = 0;

		unsigned char* _tex_data = nullptr;

	};

	namespace TEXTURE_MANAGEMENT
	{
		std::vector<ITextureBase*>& GetUninitTextures();
	}

	class ITexture2DManagement 
	{
	public:
		~ITexture2DManagement();

		typedef size_t TextureID;

		static ITexture2DManagement& Instance();

		ITexture2D* CreateTexture(const std::string& path, unsigned int tex_break_width_num = 0, unsigned int tex_break_height_num = 0);
		ITexture2D* CreateTexture(const std::string& name, const std::string& path, unsigned int tex_break_width_num = 0, unsigned int tex_break_height_num = 0);
		ITexture2D* CreateTexture(const std::string& name, const CharCharacter character, unsigned int tex_break_width_num = 0, unsigned int tex_break_height_num = 0);
		TextureID CreateTexture(ITexture2D* texture, const std::string& path, unsigned int tex_break_width_num = 0, unsigned int tex_break_height_num = 0);
		TextureID CreateTexture(ITexture2D* texture, const std::string& name, const std::string& path, unsigned int tex_break_width_num = 0, unsigned int tex_break_height_num = 0);
		TextureID CreateTexture(ITexture2D* texture, const std::string& name, const CharCharacter character, unsigned int tex_break_width_num = 0, unsigned int tex_break_height_num = 0);

		TextureID CreateTextureDynamic(const std::string& path, unsigned int tex_break_width_num = 0, unsigned int tex_break_height_num = 0);
		TextureID CreateTextureDynamic(const std::string& name, const std::string& path, unsigned int tex_break_width_num = 0, unsigned int tex_break_height_num = 0);
		TextureID CreateTextureDynamic(const std::string& name, const CharCharacter character, unsigned int tex_break_width_num = 0, unsigned int tex_break_height_num = 0);

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
		std::unordered_map<std::string, std::pair<ITexture2D*,size_t>> _textrues;
		std::vector<ITexture2D*> _vector_textrues;

		std::mutex _mutex;

	};
}

#endif // !_IITEXTURE_

