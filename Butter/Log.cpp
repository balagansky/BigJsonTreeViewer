#include "Log.h"
#include "LoggerImpl.h"

Log::DefaultAny Log::Info(const std::string& msg)
{
	LoggerHooks::gWriteLineFn(msg.c_str());
	return {};
}

Log::DefaultAny Log::Error(const std::string& msg)
{
	LoggerHooks::gWriteLineFn((std::string("ERROR: ") + msg).c_str());
	return {};
}
