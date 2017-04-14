
#include <iostream>
#include <string>
//#include <cstdarg>
//#include <vector>
//#include <cassert>
#include <mutex>

#define E_LOGGER_DLL_EXPORT __declspec(dllexport)
#include "Log.h"

/*
namespace {

	// http://stackoverflow.com/questions/69738/c-how-to-get-fprintf-results-as-a-stdstring-w-o-sprintf#69911
	std::string vformat(const char *fmt, va_list ap)
	{
		if (fmt == nullptr || *fmt == '\0')
		{
			return std::string();
		}

		// Allocate a buffer on the stack that's big enough for us almost
		// all the time.  Be prepared to allocate dynamically if it doesn't fit.
		size_t size = 1024;
		char stackbuf[1024];
		std::vector<char> dynamicbuf;
		char *buf = &stackbuf[0];

		while (1) {
			// Try to vsnprintf into our buffer.
#ifdef _WIN32
			int needed = vsnprintf_s(buf, size, _TRUNCATE, fmt, ap);
#else
			int needed = vsnprintf(buf, size, fmt, ap);
#endif
			
			// NB. C99 (which modern Linux and OS X follow) says vsnprintf
			// failure returns the length it would have needed.  But older
			// glibc and current Windows return -1 for failure, i.e., not
			// telling us how much was needed.

			if (needed <= (int)size && needed >= 0) {
				// It fit fine so we're done.
				return std::string(buf, (size_t)needed);
			}

			// vsnprintf reported that it wanted to write more characters
			// than we allotted.  So try again using a dynamic buffer.  This
			// doesn't happen very often if we chose our initial size well.
			size = (needed > 0) ? (needed + 1) : (size * 2);
			dynamicbuf.resize(size);
			buf = &dynamicbuf[0];
		}
	}

	std::string formatMessage(const char *func, int line, const char* fmt, va_list ap)
	{
		return std::string(func) + "(" + std::to_string(line) + "): " + vformat(fmt, ap);
	}
}

void log::print(LogType type, const char *func, int line, const char* fmt, ...)
{
	std::string msg;
	va_list ap;
	va_start(ap, fmt);
	msg = formatMessage(func, line, fmt, ap);
	va_end(ap);

	(type == LOG_ERROR ? std::cerr : std::cout) << msg << std::endl;
}

ArgErrorLog::ArgErrorLog(const char *func, int line) :
	m_func(func),
	m_line(line),
	m_msg("bad arguments ")
{
}

ArgErrorLog::~ArgErrorLog()
{
	assert(m_msg.size() > 2);
	m_msg.erase(m_msg.size() - 2, 2);
	Log::log(LOG_ERROR, m_func, m_line, m_msg.c_str());
}
*/

namespace {
	std::mutex s_log_mutex;
}

void eLogger::info(const char *func, int line, const eLogger::Token &t)
{
	std::lock_guard<std::mutex> lock(s_log_mutex);
	std::cout << func << "(" << line << "): " << t.m_text << '\n';
}

void eLogger::warning(const char *func, int line, const eLogger::Token &t)
{
	std::lock_guard<std::mutex> lock(s_log_mutex);
	std::cout << func << "(" << line << "): " << t.m_text << '\n';
}

void eLogger::error(const char *func, int line, const eLogger::Token &t)
{
	std::lock_guard<std::mutex> lock(s_log_mutex);
	std::cerr << func << "(" << line << "): " << t.m_text << '\n';
}
