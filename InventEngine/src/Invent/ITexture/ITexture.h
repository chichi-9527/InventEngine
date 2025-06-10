#ifndef _IITEXTURE_
#define _IITEXTURE_

#include <string>
#include <unordered_map>

namespace INVENT
{
	class ITexture2D 
	{
		friend class ITexture2DManagement;
	public:
		~ITexture2D();

		void Bind(unsigned int slot);

	private:
		ITexture2D();
		ITexture2D(const std::string& path);

	private:
		unsigned int _width;
		unsigned int _height;

		unsigned int _texture_id = 0;

	};

	class ITexture2DManagement 
	{
	public:
		~ITexture2DManagement();

		static ITexture2DManagement& Instance();

		ITexture2D* CreateTexture(const std::string& path);
		ITexture2D* CreateTexture(const std::string& name, const std::string& path);

		ITexture2D* CreateWhiteTexture();

		ITexture2D* GetTexture(const std::string& name);

		static ITexture2D* GetWhiteTexture();

	private:
		ITexture2DManagement() = default;

	private:
		std::unordered_map<std::string, ITexture2D*> _textrues;

	};
}

#endif // !_IITEXTURE_

