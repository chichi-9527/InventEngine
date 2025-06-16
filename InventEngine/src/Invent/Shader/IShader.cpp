#include "IEpch.h"
#include "IShader.h"

#include "IDefaultShader.h"

#include <filesystem>

namespace INVENT
{
	void IShader::Bind() const
	{
#ifdef USE_OPENGL
		glUseProgram(_shader_program);
#endif // USE_OPENGL
	}

	void IShader::UnBind() const
	{
#ifdef USE_OPENGL
		glUseProgram(0);
#endif // USE_OPENGL
	}

	void IShader::SetInt(const std::string & name, int value) const
	{}

	void IShader::SetIntArray(const std::string & name, int* values, unsigned int count) const
	{}

	void IShader::SetFloat(const std::string & name, float value) const
	{}

	void IShader::SetFloat2(const std::string & name, const glm::vec2 & value) const
	{}

	void IShader::SetFloat3(const std::string & name, const glm::vec3 & value) const
	{}

	void IShader::SetFloat4(const std::string & name, const glm::vec4 & value) const
	{}

	void IShader::SetMat4(const std::string & name, const glm::mat4 & value) const
	{
#ifdef USE_OPENGL
		glUniformMatrix4fv(glGetUniformLocation(_shader_program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
#endif // USE_OPENGL
	}

	void IShader::ReadFile(const std::string & file_path, std::string& file_content)
	{
		std::ifstream ifs(file_path.c_str());
		std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

		INVENT_LOG_DEBUG(content);

		file_content = content;
	}

	IShader::IShader(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
#ifdef USE_OPENGL
		_shader_program = glCreateProgram();

		auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		const GLchar* source = vertexSrc.c_str();
		glShaderSource(vertex_shader, 1, &source, NULL);
		glCompileShader(vertex_shader);

		auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		source = fragmentSrc.c_str();
		glShaderSource(fragment_shader, 1, &source, NULL);
		glCompileShader(fragment_shader);

		glAttachShader(_shader_program, vertex_shader);
		glAttachShader(_shader_program, fragment_shader);
		glLinkProgram(_shader_program);

		int suc = 0;
		glGetProgramiv(_shader_program, GL_LINK_STATUS, &suc);
		if (!suc)
			INVENT_LOG_ERROR("ERROR::SHADER::VERTEX_OR_FRAGRAM::COMPILATION_FAILED \n");

		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
#endif // USE_OPENGL
		
	}


	///////////////////////////////////////////////

	IShaderManagement& IShaderManagement::Instance()
	{
		static IShaderManagement shaders;
		return shaders;
	}

	IShader* IShaderManagement::Load(const std::string& vertexPath, const std::string& fragmentPath)
	{
		auto startcount = vertexPath.find_last_of("/\\") + 1;
		auto lastcount = vertexPath.find_last_of('.');
		std::string name = vertexPath.substr(startcount, lastcount - startcount);
		if (name.empty())
		{
			INVENT_LOG_WARNING("name is empty");
		}
		IShader* shader = Get(name);
		if (shader)
			return shader;

		// get shader file srouce
		std::string vertexSrc, fragmentSrc;

		IShader::ReadFile(vertexPath, vertexSrc);
		IShader::ReadFile(fragmentPath, fragmentSrc);

		shader = new IShader(vertexSrc, fragmentSrc);
		shader->_name = name;
		_shaders[name] = shader;
		return shader;
	}

	IShader* IShaderManagement::Load(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		IShader* shader = Get(name);
		if (shader)
			return shader;
		shader = new IShader(vertexSrc, fragmentSrc);
		shader->_name = name;
		_shaders[name] = shader;
		return shader;
	}

	IShader* IShaderManagement::Get(const std::string& name)
	{
		if (_shaders.find(name) != _shaders.end())
			return _shaders[name];
		return nullptr;
	}

	IShader* IShaderManagement::GetDefaultSquare2DShader()
	{
		static INVENT::IShader* _default_square_2d = INVENT::IShaderManagement::Instance().Load("DefaultSquare2D", std::string(IDefaultShader::DefaultSquare2DVertexShader), std::string(IDefaultShader::DefaultSquare2DFragmentShader));
		return _default_square_2d;
	}

	IShaderManagement::~IShaderManagement()
	{
		for (auto& kv : _shaders)
		{
			if (kv.second)
			{
				delete kv.second;
				kv.second = nullptr;
			}
		}
	}

}