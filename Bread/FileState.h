#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <string_view>

struct JsonNode
{
	const char* name;

	const char* value;
	std::vector<const JsonNode*> children;
	
	const JsonNode* parent;

	~JsonNode() {
		for (const JsonNode* child : children) delete child;
	}
};

struct JsonFile
{
	std::shared_ptr<const JsonNode> root;
	std::unordered_map<std::string_view, std::string> strings;
};
