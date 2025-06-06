#ifndef _I_LOG_
#define _I_LOG_

#include <string>


namespace INVENT
{
	const constexpr char* out_color_red = "\033[31m";
	const constexpr char* out_color_green = "\033[32m";
	const constexpr char* out_color_yellow = "\033[33m";
	const constexpr char* out_color_blue = "\033[34m";
	const constexpr char* out_color_magenta = "\033[35m";
	const constexpr char* out_color_cyan = "\033[36m";
	const constexpr char* out_color_white = "\033[37m";
	const constexpr char* out_color_reset = "\033[0m";

	class ILog
	{
	public:
		
		static ILog& Instance();

		void IERROR(std::string log);

		void ITRACE(std::string log);

		void IDEBUG(std::string log);

		void IINFO(std::string log);

		void IWARNING(std::string log);

		void IFATAL(std::string log);

	private:
		ILog();
		void Init();

	};

} // namespace Invent

#ifdef WATCH_LOG
#define INVENT_LOG_ERROR(str)		INVENT::ILog::Instance().IERROR(str)
#define INVENT_LOG_TRACE(str)		INVENT::ILog::Instance().ITRACE(str)
#define INVENT_LOG_DEBUG(str)		INVENT::ILog::Instance().IDEBUG(str)
#define INVENT_LOG_INFO(str)		INVENT::ILog::Instance().IINFO(str)
#define INVENT_LOG_WARNING(str)		INVENT::ILog::Instance().IWARNING(str)
#define INVENT_LOG_FATAL(str)		INVENT::ILog::Instance().IFATAL(str)
#else
#define INVENT_LOG_ERROR(str)
#define INVENT_LOG_TRACE(str)
#define INVENT_LOG_DEBUG(str)
#define INVENT_LOG_INFO(str)
#define INVENT_LOG_WARNING(str)
#define INVENT_LOG_FATAL(str)
#endif // WATCH_LOG

#define INVENT_ASSERT(is_true,false_str) if(!is_true) ILog::Instance().IFATAL(false_str);

#endif // !_I_LOG_

