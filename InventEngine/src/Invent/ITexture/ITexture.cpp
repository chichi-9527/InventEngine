#include "IEpch.h"
#include "ITexture.h"

#include "IEngine.h"
#include "IBase/IWindow.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace INVENT
{

	static std::unordered_map<std::string, std::pair<ITextureBase*, size_t>> _textrues;
	static std::vector<ITextureBase*> _vector_textrues;
	static std::mutex _mutex;


	ITextureBase::~ITextureBase()
	{
#ifdef USE_OPENGL
		glDeleteTextures(1, &_texture_id);
#endif // USE_OPENGL
	}

	void ITextureBase::BindUnit(unsigned int slot) const
	{
#ifdef USE_OPENGL
		glBindTextureUnit(slot, _texture_id);
#endif // USE_OPENGL
	}

	ITexture2D::~ITexture2D()
	{
	}

	void ITexture2D::Bind() const
	{
#ifdef USE_OPENGL
		glBindTexture(GL_TEXTURE_2D, _texture_id);
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
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // 默认对齐

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
			else if (1 == _channels)
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

				glTextureStorage2D(_texture_id, 1, GL_R8, _width, _height);
				glTextureSubImage2D(_texture_id, 0, 0, 0, _width, _height, GL_RED, GL_UNSIGNED_BYTE, _tex_data);
			}
			glTextureParameteri(_texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(_texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureParameteri(_texture_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(_texture_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

#endif // USE_OPENGL
			if (1 != _channels)
			{
				stbi_image_free(_tex_data);
			}
			else
			{
				delete[] _tex_data;
			}
			_tex_data = nullptr;
			IsValid = true;
		}
		else
			INVENT_LOG_WARNING(std::string("texture init error: _tex_data is null ; TEXTURE: ") + this->Name());
	}

	ITexture2D::ITexture2D()
		: ITextureBase()
		, _width(1)
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
		
		_type = ITextureBase::TextureType::TEXTURE_2D;
		IsValid = true;
	}

	ITexture2D::ITexture2D(const std::string& name, const std::string& path, const _UInt2& breakup)
		: ITextureBase()
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

		TEXTURE_MANAGEMENT::GetUninitTextures().push_back((ITextureBase*)this);
		_type = ITextureBase::TextureType::TEXTURE_2D;
	}

	ITexture2D::ITexture2D(const std::string& name, const CharCharacter& character, const _UInt2& breakup)
		: ITextureBase()
	{
		_name = name;
		_texture_breakup = breakup;
		if (_texture_breakup.IsZore())
			_texture_breakup.is_valid = false;

		_width = character.Width;
		_height = character.Rows;
		_channels = 1;
		_tex_data = new unsigned char[(size_t)_width * (size_t)_height];
		memcpy(_tex_data, character.Buffer, (size_t)_width * (size_t)_height);
		_charcharacter = character;

		TEXTURE_MANAGEMENT::GetUninitTextures().push_back((ITextureBase*)this);
		_type = ITextureBase::TextureType::TEXTURE_2D;
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
		_textrues.clear();
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

	ITexture2D* ITexture2DManagement::CreateTexture(const std::string& name, const CharCharacter character, unsigned int tex_break_width_num, unsigned int tex_break_height_num)
	{
		ITexture2D* texture = GetTexture(name);
		if (texture)
			return texture;
		texture = new ITexture2D(name, character, ITexture2D::_UInt2(tex_break_width_num, tex_break_height_num));
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
			auto t = (*tex).second.first;
			if (t->Type() != ITextureBase::TextureType::TEXTURE_2D)
			{
				INVENT_LOG_ERROR(std::string("Create texture name already exists, but the name is not 2D texture: ") + name);
				return 0;
			}
			texture = static_cast<ITexture2D*>(t);
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

	ITexture2DManagement::TextureID ITexture2DManagement::CreateTexture(ITexture2D* texture, const std::string& name, const CharCharacter character, unsigned int tex_break_width_num, unsigned int tex_break_height_num)
	{
		auto tex = _textrues.find(name);
		if (tex != _textrues.end())
		{
			auto t = (*tex).second.first;
			if (t->Type() != ITextureBase::TextureType::TEXTURE_2D)
			{
				INVENT_LOG_ERROR(std::string("Create texture name already exists, but the name is not 2D texture: ") + name);
				return 0;
			}
			texture = static_cast<ITexture2D*>(t);
			return (*tex).second.second;
		}

		texture = new ITexture2D(name, character, ITexture2D::_UInt2(tex_break_width_num, tex_break_height_num));

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

		return CreateTextureDynamic(name, path, tex_break_width_num, tex_break_height_num);
	}

	ITexture2DManagement::TextureID ITexture2DManagement::CreateTextureDynamic(const std::string& name, const std::string& path, unsigned int tex_break_width_num, unsigned int tex_break_height_num)
	{
		auto tex = _textrues.find(name);
		if (tex != _textrues.end())
		{
			return (*tex).second.second;
		}

		size_t id = 0;
		{
			std::lock_guard<std::mutex> lock(_mutex);
			id = _vector_textrues.size();
			_vector_textrues.push_back(nullptr);
			_textrues[name] = { nullptr, id };
		}

		IEngine::InstancePtr()->GetIWindow()->GetThreadPool()->Submit(0, [this, tex_break_width_num, tex_break_height_num, id](const std::string& Name, const std::string& Path) {
			auto texture = new ITexture2D(Name, Path, ITexture2D::_UInt2(tex_break_width_num, tex_break_height_num));
			std::lock_guard<std::mutex> lock(_mutex);
			_vector_textrues[id] = (ITextureBase*)texture;
			_textrues[Name].first = (ITextureBase*)texture;
			}, name, path);

		return id;
	}

	ITexture2DManagement::TextureID ITexture2DManagement::CreateTextureDynamic(const std::string& name, const CharCharacter character, unsigned int tex_break_width_num, unsigned int tex_break_height_num)
	{
		auto tex = _textrues.find(name);
		if (tex != _textrues.end())
		{
			return (*tex).second.second;
		}

		size_t id = 0;
		{
			std::lock_guard<std::mutex> lock(_mutex);
			id = _vector_textrues.size();
			_vector_textrues.push_back(nullptr);
			_textrues[name] = { nullptr, id };
		}

		IEngine::InstancePtr()->GetIWindow()->GetThreadPool()->Submit(0, [this, tex_break_width_num, tex_break_height_num, id](const std::string& Name, const CharCharacter& Path) {
			auto texture = new ITexture2D(Name, Path, ITexture2D::_UInt2(tex_break_width_num, tex_break_height_num));
			std::lock_guard<std::mutex> lock(_mutex);
			_vector_textrues[id] = (ITextureBase*)texture;
			_textrues[Name].first = (ITextureBase*)texture;
			}, name, character);

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
		{
			if (_textrues[name].first->Type() == ITextureBase::TextureType::TEXTURE_2D)
				return (ITexture2D*)_textrues[name].first;
			INVENT_LOG_WARNING(std::string("Find texture name ok, but the name is not 2D texture: ") + name);
		}
		
		return nullptr;
	}

	ITexture2D* ITexture2DManagement::GetTexture(TextureID id)
	{
		if (id && id < _vector_textrues.size())
		{
			auto texture = _vector_textrues[id];
			if (texture->Type() == ITextureBase::TextureType::TEXTURE_2D)
				return (ITexture2D*)texture;
			INVENT_LOG_WARNING(std::string("Find texture id ok, but the name is not 2D texture: ") + std::to_string(id));
		}
		return nullptr;
	}

	ITexture2D* ITexture2DManagement::GetWhiteTexture()
	{
		static ITexture2D* white_texture = ITexture2DManagement::Instance().CreateWhiteTexture();
		return white_texture;
	}

	ITexture2DManagement::ITexture2DManagement()
	{
		if (_vector_textrues.empty())
			_vector_textrues.push_back(nullptr);
	}

	/// <summary>
	/// ////////////  ITextureCubeMap  ////////////////////////////////////////////
	/// </summary>
	
	ITextureCubeMap::~ITextureCubeMap()
	{}

	ITextureCubeMap::ITextureCubeMap(const std::string& name, const std::vector<std::string>& face_pathes)
		: ITextureBase()
	{
		_name = name;

		auto size = face_pathes.size();
		if (0 == size)return;
		_face_num = (unsigned int)size;

		_tex_data = new unsigned char* [size];

		int width = 0, height = 0, channels = 0;
		_tex_data[0] = stbi_load(face_pathes[0].c_str(), &width, &height, &channels, 0);
		_width = (unsigned int)width;
		_height = (unsigned int)height;
		_channels = (unsigned int)channels;
		if (_width != _height)
		{
			INVENT_LOG_WARNING(std::string("TEXTURE_CUBEMAP: _width != _height: ") + _name);
		}

		for (unsigned int i = 1; i < _face_num; ++i)
		{
			int width = 0, height = 0, channels = 0;
			_tex_data[i] = stbi_load(face_pathes[i].c_str(), &width, &height, &channels, 0);
			if (_width != width || _height != height || _channels != channels)
			{
				INVENT_LOG_ERROR(std::string("TEXTURE_CUBEMAP: _width != width || _height != height || _channels != channels: ") + _name);
				return;
			}
			
		}

		TEXTURE_MANAGEMENT::GetUninitTextures().push_back((ITextureBase*)this);
		_type = ITextureBase::TextureType::TEXTURE_CUBE_MAP;
	}

	void ITextureCubeMap::Bind() const
	{
#ifdef USE_OPENGL
		glBindTexture(GL_TEXTURE_CUBE_MAP, _texture_id);
#endif // USE_OPENGL
	}

	void ITextureCubeMap::InitTextureID()
	{
#ifdef USE_OPENGL
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &_texture_id);

		if (_face_num)
		{

			if (4 == _channels)
			{
				glTextureStorage2D(_texture_id, 1, GL_RGBA8, _width, _height);
				glTextureParameteri(_texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTextureParameteri(_texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTextureParameteri(_texture_id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				for (unsigned int i = 0; i < _face_num; ++i)
				{
					glTextureSubImage3D(_texture_id, 0, 0, 0, i, _width, _height, 1, GL_RGBA, GL_UNSIGNED_BYTE, _tex_data[i]);
				}
				
			}
			else if(3 == _channels)
			{
				glTextureStorage2D(_texture_id, 1, GL_RGB8, _width, _height);
				glTextureParameteri(_texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTextureParameteri(_texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTextureParameteri(_texture_id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				for (unsigned int i = 0; i < _face_num; ++i)
				{
					glTextureSubImage3D(_texture_id, 0, 0, 0, i, _width, _height, 1, GL_RGB, GL_UNSIGNED_BYTE, _tex_data[i]);
				}
			}
			else
			{
				INVENT_LOG_ERROR(std::string("TEXTURE_CUBEMAP: channels not 3 and not 4: ") + _name);
				return;
			}
		}

#endif // USE_OPENGL

		IsValid = true;

	}


	static std::vector<ITextureBase*> UninitTextrues;

	std::vector<ITextureBase*>& TEXTURE_MANAGEMENT::GetUninitTextures()
	{
		return UninitTextrues;
	}

	/// <summary>
	/// ////////////  ITextureManagement  ////////////////////////////////////////////
	/// </summary>

	ITextureManagement::~ITextureManagement()
	{
		
	}

	ITextureManagement::ITextureManagement()
	{
		if (_vector_textrues.empty())
			_vector_textrues.push_back(nullptr);
	}

	ITextureManagement& ITextureManagement::Instance()
	{
		static ITextureManagement m;
		return m;
	}

}