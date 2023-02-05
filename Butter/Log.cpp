#include "Log.h"
#include "LoggerImpl.h"

void Log::Info(const std::string &msg)
{
	LoggerHooks::gWriteLineFn(msg.c_str());
}
