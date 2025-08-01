#include "IEpch.h"
#include "ITexture.h"

#include "IEngine.h"

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

	void ITexture2D::Bind() const
	{
#ifdef USE_OPENGL
		glBindTexture(GL_TEXTURE_2D, _texture_id);
#endif // USE_OPENGL
	}

	void ITexture2D::BindUnit(unsigned int slot) const
	{
#ifdef USE_OPENGL
		glBindTextureUnit(slot, _texture_id);
#endif // USE_OPENGL
	}

	void ITexture2D::SetBreakNum(unsigned int w, unsigned int h)
	{
		if (w == 0 || h == 0)
			_texture_breakup.is_valid = false; return;
		_texture_breakup.width = w;
		_texture_breakup.height = h;
		_texture_breakup.is_valid = true;
	}

	void ITexture2D::SetBreakNum(const _UInt2 & break_num)
	{
		if(break_num.IsZore() || !break_num.is_valid)
			_texture_breakup.is_valid = false; return;
		_texture_breakup = break_num;
	}

	void ITexture2D::InitTextureID()
	{
		if (_tex_data)
		{
#ifdef USE_OPENGL
			glCreateTextures(GL_TEXTURE_2D, 1, &_texture_id);

			if (4 == _channels)
			{
				glTextureStorage2D(_texture_id, 1, GL_RGBA8, _width, _height);
				glTextureSubImage2D(_texture_id, 0, 0, 0, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, _tex_data);
			}
			else if (3 == _channels)
			{
				glTextureStorage2D(_texture_id, 1, GL_RGB8, _width, _height);
				glTextureSubImage2D(_texture_id, 0, 0, 0, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, _tex_data);
			}
			glTextureParameteri(_texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(_texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureParameteri(_texture_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(_texture_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

#endif // USE_OPENGL

			stbi_image_free(_tex_data);
		}

		IsValid = true;
	}

	ITexture2D::ITexture2D()
		: _width(1)
		, _height(1)
	{
		_texture_breakup.is_valid = false;
		_name = "DefaultWhiteTexture";

		unsigned int _color_white = 0xffffffff;

#ifdef USE_OPENGL
		glCreateTextures(GL_TEXTURE_2D, 1, &_texture_id);

		glTextureStorage2D(_texture_id, 1, GL_RGBA8, _width, _height);

		glTextureParameteri(_texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(_texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(_texture_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(_texture_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(_texture_id, 0, 0, 0, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, (void*)&_color_white);
#endif // USE_OPENGL

		IsValid = true;
	}

	ITexture2D::ITexture2D(const std::string& name, const std::string& path, const _UInt2& breakup)
	{
		_name = name;
		_texture_breakup = breakup;
		if (_texture_breakup.IsZore()) 
			_texture_breakup.is_valid = false;

		int width = 0, height = 0, channels = 0;
		stbi_set_flip_vertically_on_load(1);

		_tex_data = stbi_load(path.c_str(), &width, &height, &channels, 0);

		_width = width;
		_height = height;
		_channels = channels;

		ITexture2DManagement::Instance().GetUninitTextures().push_back(this);
		
	}

	/// <summary>
	/// ////////////  ITexture2DManagement  ////////////////////////////////////////////
	/// </summary>

	ITexture2DManagement::~ITexture2DManagement()
	{
		for (auto& kv : _textrues)
		{
			if (kv.second.first)
			{
				delete kv.second.first;
				kv.second.first = nullptr;
			}
		}

		_vector_textrues.clear();
	}

	ITexture2DManagement& ITexture2DManagement::Instance()
	{
		static ITexture2DManagement management;
		return management;
	}

	ITexture2D* ITexture2DManagement::CreateTexture(const std::string& path, unsigned int tex_break_width_num, unsigned int tex_break_height_num)
	{
		auto startcount = path.find_last_of("/\\") + 1;
		auto lastcount = path.find_last_of('.');
		std::string name = path.substr(startcount, lastcount - startcount);
		if (name.empty())
		{
			INVENT_LOG_WARNING("name is empty");
			name = "Empty";
		}
		
		return CreateTexture(name, path, tex_break_width_num, tex_break_height_num);
	}

	ITexture2D* ITexture2DManagement::CreateTexture(const std::string& name, const std::string& path, unsigned int tex_break_width_num, unsigned int tex_break_height_num)
	{
		ITexture2D* texture = GetTexture(name);
		if (texture)
			return texture;
		texture = new ITexture2D(name, path, ITexture2D::_UInt2(tex_break_width_num, tex_break_height_num));
		size_t tex_id = 0;
		{
			std::lock_guard<std::mutex> lock(_mutex);
			tex_id = _vector_textrues.size();
			_vector_textrues.push_back(texture);
		}

		_textrues[name] = { texture, tex_id };
		return texture;
	}

	ITexture2DManagement::TextureID ITexture2DManagement::CreateTexture(ITexture2D* texture, const std::string& path, unsigned int tex_break_width_num, unsigned int tex_break_height_num)
	{
		auto startcount = path.find_last_of("/\\") + 1;
		auto lastcount = path.find_last_of('.');
		std::string name = path.substr(startcount, lastcount - startcount);
		if (name.empty())
		{
			INVENT_LOG_WARNING("name is empty");
			name = "Empty";
		}
		
		return CreateTexture(texture, name, path, tex_break_width_num, tex_break_height_num);
	}

	ITexture2DManagement::TextureID ITexture2DManagement::CreateTexture(ITexture2D* texture, const std::string& name, const std::string& path, unsigned int tex_break_width_num, unsigned int tex_break_height_num)
	{
		auto tex = _textrues.find(name);
		if (tex != _textrues.end())
		{
			texture = (*tex).second.first;
			return (*tex).second.second;
		}

		texture = new ITexture2D(name, path, ITexture2D::_UInt2(tex_break_width_num, tex_break_height_num));

		size_t tex_id = 0;
		{
			std::lock_guard<std::mutex> lock(_mutex);
			tex_id = _vector_textrues.size();
			_vector_textrues.push_back(texture);
		}

		_textrues[name] = { texture, tex_id };
		return  tex_id;
	}

	ITexture2DManagement::TextureID ITexture2DManagement::CreateTextureDynamic(const std::string& path, unsigned int tex_break_width_num, unsigned int tex_break_height_num)
	{
		auto startcount = path.find_last_of("/\\") + 1;
		auto lastcount = path.find_last_of('.');
		std::string name = path.substr(startcount, lastcount - startcount);

		auto tex = _textrues.find(name);
		if (tex != _textrues.end())
		{
			return (*tex).second.second;
		}

		return CreateTextureDynamic(name, path, tex_break_width_num, tex_break_height_num);
	}

	ITexture2DManagement::TextureID ITexture2DManagement::CreateTextureDynamic(const std::string& name, const std::string& path, unsigned int tex_break_width_num, unsigned int tex_break_height_num)
	{
		size_t id = 0;
		{
			std::lock_guard<std::mutex> lock(_mutex);
			id = _vector_textrues.size();
			_vector_textrues.push_back(nullptr);
		}

		IEngine::InstancePtr()->GetIWindow()->GetThreadPool()->Submit(0, [this, tex_break_width_num, tex_break_height_num, id](const std::string& Name, const std::string& Path) {
			auto texture = new ITexture2D(Name, Path, ITexture2D::_UInt2(tex_break_width_num, tex_break_height_num));
			std::lock_guard<std::mutex> lock(_mutex);
			this->_vector_textrues[id] = texture;
			_textrues[Name] = { texture, id };
			}, name, path);

		return id;
	}

	ITexture2D* ITexture2DManagement::CreateWhiteTexture()
	{
		ITexture2D* texture = GetTexture("DefaultWhiteTexture");
		if (texture)
			return texture;
		ITexture2D* white_texture = new ITexture2D;
		std::lock_guard<std::mutex> lock(_mutex);
		_vector_textrues.push_back(texture);
		_textrues["DefaultWhiteTexture"] = { white_texture, _vector_textrues.size() - 1 };
		return white_texture;
	}

	ITexture2D* ITexture2DManagement::GetTexture(const std::string& name)
	{
		if (_textrues.find(name) != _textrues.end())
			return _textrues[name].first;
		return nullptr;
	}

	ITexture2D* ITexture2DManagement::GetWhiteTexture()
	{
		static ITexture2D* white_texture = ITexture2DManagement::Instance().CreateWhiteTexture();
		return white_texture;
	}

	ITexture2DManagement::ITexture2DManagement()
	{
		_vector_textrues.push_back(nullptr);
	}
}