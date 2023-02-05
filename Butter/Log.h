#pragma once

#include <string>

namespace Log
{
	// for convenience, these always return false
	struct DefaultAny {
		template <typename T>
		operator T() { return {}; }
	};

	DefaultAny Info(const std::string &msg);
	DefaultAny Error(const std::string &msg);
}