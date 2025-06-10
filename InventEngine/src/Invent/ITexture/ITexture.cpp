#include "IEpch.h"
#include "ITexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace INVENT
{
	ITexture2D::~ITexture2D()
	{
#ifdef USE_OPENGL
		glDeleteTextures(1, &_texture_id);
#endif // USE_OPENGL

	}

	void ITexture2D::Bind(unsigned int slot)
	{
#ifdef USE_OPENGL
		glBindTextureUnit(slot, _texture_id);
#endif // USE_OPENGL
	}

	ITexture2D::ITexture2D()
		: _width(1)
		, _height(1)
	{
		unsigned int white = 0xffffffff;
#ifdef USE_OPENGL
		glCreateTextures(GL_TEXTURE_2D, 1, &_texture_id);

		glTextureStorage2D(_texture_id, 1, GL_RGBA8, _width, _height);

		glTextureParameteri(_texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(_texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(_texture_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(_texture_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(_texture_id, 0, 0, 0, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, (void*)white);
#endif // USE_OPENGL
	}

	ITexture2D::ITexture2D(const std::string& path)
	{
		int width = 0, height = 0, channels = 0;
		stbi_set_flip_vertically_on_load(1);

		auto data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		if (data)
		{
			INVENT_LOG_INFO("read data ok \n");

			_width = (unsigned int)width;
			_height = (unsigned int)height;

#ifdef USE_OPENGL
			glCreateTextures(GL_TEXTURE_2D, 1, &_texture_id);

			if (4 == channels)
			{
				glTextureStorage2D(_texture_id, 1, GL_RGBA8, width, height);
				glTextureSubImage2D(_texture_id, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}
			else if (3 == channels)
			{
				glTextureStorage2D(_texture_id, 1, GL_RGB8, width, height);
				glTextureSubImage2D(_texture_id, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			glTextureParameteri(_texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(_texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureParameteri(_texture_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(_texture_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

#endif // USE_OPENGL

			stbi_image_free(data);
		}
		_width = 0;
		_height = 0;
	}

	/// <summary>
	/// ////////////  ITexture2DManagement  ////////////////////////////////////////////
	/// </summary>

	ITexture2DManagement::~ITexture2DManagement()
	{
		for (auto& kv : _textrues)
		{
			if (kv.second)
			{
				delete kv.second;
				kv.second = nullptr;
			}
		}
	}

	ITexture2DManagement& ITexture2DManagement::Instance()
	{
		static ITexture2DManagement management;
		return management;
	}

	ITexture2D* ITexture2DManagement::CreateTexture(const std::string& path)
	{
		auto startcount = path.find_last_of("/\\") + 1;
		auto lastcount = path.find_last_of('.');
		std::string name = path.substr(startcount, lastcount - startcount);
		if (name.empty())
		{
			INVENT_LOG_WARNING("name is empty");
		}
		
		return CreateTexture(name, path);
	}

	ITexture2D* ITexture2DManagement::CreateTexture(const std::string& name, const std::string& path)
	{
		ITexture2D* texture = GetTexture(name);
		if (texture)
			return texture;
		texture = new ITexture2D(path);
		return texture;
	}

	ITexture2D* ITexture2DManagement::CreateWhiteTexture()
	{
		ITexture2D* texture = GetTexture("DefaultWhiteTexture");
		if (texture)
			return texture;
		ITexture2D* white_texture = new ITexture2D;
		_textrues["DefaultWhiteTexture"] = white_texture;
		return white_texture;
	}

	ITexture2D* ITexture2DManagement::GetTexture(const std::string& name)
	{
		if (_textrues.find(name) != _textrues.end())
			return _textrues[name];
		return nullptr;
	}

	ITexture2D* ITexture2DManagement::GetWhiteTexture()
	{
		static ITexture2D* white_texture = ITexture2DManagement::Instance().CreateWhiteTexture();
		return white_texture;
	}
}