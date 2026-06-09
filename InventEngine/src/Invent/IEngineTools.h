#ifndef _IENGINETOOLS_
#define _IENGINETOOLS_

#include <memory>
#include <string>

namespace INVENT
{
	class IEngineTools
	{
		IEngineTools();
	public:
		~IEngineTools();

		static const std::string& GetRunPath();
		static std::vector<char> ReadFile(const std::string& path);

		static IEngineTools& Instance();
		



	private:


	};
}

#endif // !_IENGINETOOLS_

