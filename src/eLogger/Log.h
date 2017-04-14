
#ifndef LOG_H
#define LOG_H

#include <string>

//  __FUNCDNAME__     __FUNCSIG__     __FUNCTION__

#ifndef __FUNCTION_NAME__
#ifdef _WIN32   //WINDOWS
#define __FUNCTION_NAME__   __FUNCTION__  
#else          //*NIX
#define __FUNCTION_NAME__   __func__ 
#endif
#endif

#define LOG_INFO(msg) eLogger::info(__FUNCTION_NAME__, __LINE__, (eLogger::Token()+msg));
#define LOG_WARNING(msg) eLogger::warning(__FUNCTION_NAME__, __LINE__, (eLogger::Token()+msg));
#define LOG_ERROR(msg) eLogger::error(__FUNCTION_NAME__, __LINE__, (eLogger::Token()+msg));
//#define LOG_ARGUMENT_ERROR(arguments) ptlog::argerror(__FUNCTION_NAME__, __LINE__, (ptlog::ArgumentToken()+msg));

namespace eLogger
{
	class Token
	{
	public:
		Token() {}
		template<class T>
		Token(T n) : m_text(std::to_string(n)) {}
		Token(const std::string &str) : m_text(str) {}
		Token(const char *c) : m_text(c) {}
		Token(char *c) : m_text(c) {}
		Token(bool b) : m_text(b ? "true" : "false") {}
		Token(void* p) : m_text(std::to_string((size_t)p)) {}
		virtual ~Token() { /*delete m_text;*/ }

		virtual Token operator +(const Token &b) {
			return Token(m_text + b.m_text);
		}

		const std::string m_text;
	};

	/*class ArgumentToken : public Token
	{
	public:
		using Token::Token;
		ArgumentToken operator +(const ArgumentToken &b) {
			return ArgumentToken(m_text + ", " + b.m_text);
		}
	};*/

	 void info(const char *func, int line, const Token&);
	 void warning(const char *func, int line, const Token&);
	 void error(const char *func, int line, const Token&);
	//void argerror(const char *func, int line, const ArgumentToken&);

	// timestamps

	// sinks
};

#endif
