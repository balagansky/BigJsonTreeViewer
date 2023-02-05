#pragma once

#include <vector>

struct JsonNode
{
	const char* name;

	const char* value;
	std::vector<const JsonNode*> children;
};
