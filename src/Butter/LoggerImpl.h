#pragma once

#include <functional>

namespace LoggerHooks {
	// must be set externally
	extern std::function<void(const char*)> gWriteLineFn;
}
