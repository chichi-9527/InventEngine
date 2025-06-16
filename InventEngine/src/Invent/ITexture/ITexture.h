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

		void Bind() const;
		void BindUnit(unsigned int slot = 0) const;
		const std::string& Name() const { return _name; }

	private:
		ITexture2D();
		ITexture2D(const std::string& name, const std::string& path);

	private:
		std::string _name;

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

		ITexture2D* CreateTexture(const std::string& path);
		ITexture2D* CreateTexture(const std::string& name, const std::string& path);

		TextureID CreateTextureDynamic(const std::string& path);

		ITexture2D* CreateWhiteTexture();

		ITexture2D* GetTexture(const std::string& name);

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

