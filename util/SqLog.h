
#pragma once


/** Example usage:

	DEBUG("error: %d", errno);

will print something like

	[0.123 debug squinkylabs] error: 67
*/

#define __NDBG 

#ifdef __NDBG
	#define SQDEBUG(format, ...) (void) format
#endif

#ifdef __PLUGIN

#include "rack.hpp"

#ifndef SQDEBUG
#define SQDEBUG(format, ...) rack::logger::log(rack::logger::DEBUG_LEVEL, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif

#define SQINFO(format, ...) rack::logger::log(rack::logger::INFO_LEVEL, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#define SQWARN(format, ...) rack::logger::log(rack::logger::WARN_LEVEL, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#define SQFATAL(format, ...) rack::logger::log(rack::logger::FATAL_LEVEL, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)

#else

#ifndef SQDEBUG
#define SQDEBUG(format, ...) SqLog::log(SqLog::DEBUG_LEVEL, __FILE__, __LINE__, format, ##__VA_ARGS__)
#endif


#define SQINFO(format, ...) SqLog::log(SqLog::INFO_LEVEL, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define SQWARN(format, ...) SqLog::log(SqLog::WARN_LEVEL, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define SQFATAL(format, ...) SqLog::log(SqLog::FATAL_LEVEL, __FILE__, __LINE__, format, ##__VA_ARGS__)


namespace SqLog {

enum Level {
	DEBUG_LEVEL,
	INFO_LEVEL,
	WARN_LEVEL,
	FATAL_LEVEL
};

void log(Level level, const char* filename, int line, const char* format, ...);

} // namespace SqLog
#endif