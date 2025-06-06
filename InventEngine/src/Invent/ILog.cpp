#include "IEpch.h"
#include "ILog.h"


namespace INVENT
{
    ILog::ILog()
    {
        Init();
    }

    ILog& ILog::Instance()
    {
        std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        static ILog _ilog;
        return _ilog;
    }

	void ILog::Init()
	{
#ifdef INVENT_USE_WINDOWS
        // 启用 ANSI 转义序列支持
        auto enableVTMode = []() {
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hOut == INVALID_HANDLE_VALUE)
            {
                return;
            }

            DWORD dwMode = 0;
            if (!GetConsoleMode(hOut, &dwMode))
            {
                return;
            }

            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            if (!SetConsoleMode(hOut, dwMode))
            {
                return;
            }
            };
        enableVTMode();
#endif // !INVENT_USE_WINDOWS

        boost::log::add_common_attributes();

        // 添加控制台 sink
        auto console_sink = boost::log::add_console_log(std::clog);

        // 设置日志格式并添加颜色
        console_sink->set_formatter(
            boost::log::expressions::stream
            << boost::log::expressions::if_(boost::log::expressions::attr<boost::log::trivial::severity_level>("Severity") == boost::log::trivial::trace)
            [boost::log::expressions::stream << out_color_green]
            << boost::log::expressions::if_(boost::log::expressions::attr<boost::log::trivial::severity_level>("Severity") == boost::log::trivial::debug)
            [boost::log::expressions::stream << out_color_cyan]
            << boost::log::expressions::if_(boost::log::expressions::attr<boost::log::trivial::severity_level>("Severity") == boost::log::trivial::info)
            [boost::log::expressions::stream << out_color_white]
            << boost::log::expressions::if_(boost::log::expressions::attr<boost::log::trivial::severity_level>("Severity") == boost::log::trivial::warning)
            [boost::log::expressions::stream << out_color_yellow]
            << boost::log::expressions::if_(boost::log::expressions::attr<boost::log::trivial::severity_level>("Severity") == boost::log::trivial::error)
            [boost::log::expressions::stream << out_color_red]
            << boost::log::expressions::if_(boost::log::expressions::attr<boost::log::trivial::severity_level>("Severity") == boost::log::trivial::fatal)
            [boost::log::expressions::stream << out_color_magenta]
            << "[" << boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S") << "] "
            << "[" << boost::log::trivial::severity << "] "
            << boost::log::expressions::smessage
            << out_color_reset
        );
	}

    void ILog::IERROR(std::string log)
    {
        BOOST_LOG_TRIVIAL(error) << log.c_str();
    }
    void ILog::ITRACE(std::string log)
    {
        BOOST_LOG_TRIVIAL(trace) << log.c_str();
    }
    void ILog::IDEBUG(std::string log)
    {
        BOOST_LOG_TRIVIAL(debug) << log.c_str();
    }
    void ILog::IINFO(std::string log)
    {
        BOOST_LOG_TRIVIAL(info) << log.c_str();
    }
    void ILog::IWARNING(std::string log)
    {
        BOOST_LOG_TRIVIAL(warning) << log.c_str();
    }
    void ILog::IFATAL(std::string log)
    {
        BOOST_LOG_TRIVIAL(fatal) << log.c_str();
    }

} // namespace Invent