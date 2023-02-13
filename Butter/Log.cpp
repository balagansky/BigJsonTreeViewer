#include "Log.h"
#include "LoggerImpl.h"

Log::DefaultAny Log::Info(const std::string& msg)
{
	if (LoggerHooks::gWriteLineFn)
		LoggerHooks::gWriteLineFn(msg.c_str());
	return {};
}

Log::DefaultAny Log::Error(const std::string& msg)
{
	return Info(std::string("ERROR: ") + msg);
}

bool Log::Invariant(bool check, const char* msg)
{
	if (!check && LoggerHooks::gWriteLineFn)
		Info((std::string("ASSERTION ERROR: ") + msg
			+ " This is a bug - please report it (provide a repro if you can)!"));

	return check;
}
