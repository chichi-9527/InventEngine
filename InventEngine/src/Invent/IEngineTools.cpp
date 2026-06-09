#include <IEpch.h>
#include "IEngineTools.h"

#include <filesystem>

static auto RunPath = std::filesystem::current_path().string();

namespace INVENT
{
	IEngineTools::IEngineTools()
	{}
	IEngineTools::~IEngineTools()
	{}

	const std::string& IEngineTools::GetRunPath()
	{
		return RunPath;
	}

	std::vector<char> IEngineTools::ReadFile(const std::string& path)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);
		if (!file.is_open())
		{
			INVENT_LOG_ERROR(std::format("ERROR : [ ShaderCompiler ] failed to open file : {} \n", path));
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buf(fileSize + 1);
		file.seekg(0);
		file.read(buf.data(), fileSize);
		file.close();
		buf[fileSize] = '\0';
		return buf;
	}

	IEngineTools& IEngineTools::Instance()
	{
		static IEngineTools et;
		return et;
	}

	


}