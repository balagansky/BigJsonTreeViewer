#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <string_view>

class JsonNode
{
public:
	virtual ~JsonNode() = default;


};

struct JsonFile
{
public:
	virtual ~JsonFile() = default;

	//virtual const JsonNode *GetRoot() const = 0;
};
