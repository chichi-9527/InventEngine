#ifndef _ISHADER_
#define _ISHADER_

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

namespace INVENT
{
	class IShader 
	{
		friend class IShaderManagement;

	public:
		~IShader() = default;

		void Bind();
		void UnBind();

		void SetInt(const std::string& name, int value);
		void SetIntArray(const std::string& name, int* values, unsigned int count);
		void SetFloat(const std::string& name, float value);
		void SetFloat2(const std::string& name, const glm::vec2& value);
		void SetFloat3(const std::string& name, const glm::vec3& value);
		void SetFloat4(const std::string& name, const glm::vec4& value);
		void SetMat4(const std::string& name, const glm::mat4& value);

		const std::string& Name() const { return _name; }


	private:
		IShader(const std::string& vertexSrc, const std::string& fragmentSrc);

		static void ReadFile(const std::string& file_path, std::string& file_content);

	private:
		std::string _name;

		unsigned int _shader_program;
	};

	class IShaderManagement 
	{
	public:
		~IShaderManagement();

		static IShaderManagement& Instance();

		IShader* Load(const std::string& vertexPath, const std::string& fragmentPath);
		IShader* Load(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

		IShader* Get(const std::string& name);

		static IShader* GetDefaultSquare2DShader();

	private:
		IShaderManagement() = default;

	private:
		std::unordered_map<std::string, IShader*> _shaders;

	private:


	};

}

#endif // !_ISHADER_

