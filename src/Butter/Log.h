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
	
	// asserts check and prints msg followed by direction to report the issue
	// returns check
	bool Invariant(bool check, const char *msg);
}
