#include "LoggerImpl.h"

namespace LoggerHooks {
	std::function<void(const char*)> gWriteLineFn;
}
