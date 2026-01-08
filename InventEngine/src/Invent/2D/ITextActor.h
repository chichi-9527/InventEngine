#ifndef _ITEXTACTOR_
#define _ITEXTACTOR_
#include "IActor2D.h"

namespace INVENT
{
	class ITextActor :
		public IActor2D
	{
	public:
		std::wstring Text;

		ITextActor();
		virtual ~ITextActor();

		

	private:

	};

}
#endif //!_ITEXTACTOR_
